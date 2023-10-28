// --[[ include ]]

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>


// --[[ interface ]]

typedef uint8_t t$pctr$node;
typedef enum t$pctr$WeightSize$enum{
  e$pctr$WeightSize$bit_null=0,
  e$pctr$WeightSize$bit_8=1,
  e$pctr$WeightSize$bit_16=2,
  e$pctr$WeightSize$bit_32=4,
  e$pctr$WeightSize$bit_64=8,
} t$pctr$WeightSize$enum;
typedef struct t$pctr$WeightSize{
  const enum t$pctr$WeightSize$enum bit_null;
  const enum t$pctr$WeightSize$enum bit_8;
  const enum t$pctr$WeightSize$enum bit_16;
  const enum t$pctr$WeightSize$enum bit_32;
  const enum t$pctr$WeightSize$enum bit_64;
} t$pctr$WeightSize;
const t$pctr$WeightSize e$pctr$WeightSize = {
  .bit_null=e$pctr$WeightSize$bit_null,
  .bit_8=e$pctr$WeightSize$bit_8,
  .bit_16=e$pctr$WeightSize$bit_16,
  .bit_32=e$pctr$WeightSize$bit_32,
  .bit_64=e$pctr$WeightSize$bit_64,
};
typedef struct t$pctr$Layer{
  uint8_t weight_size;
  uint64_t node_count;
  t$pctr$node* raw_node;
} t$pctr$Layer;
typedef struct t$pctr$Layers{
  uint64_t layer_count;
  t$pctr$Layer* layers;
} t$pctr$Layers;

//? 레이어를 
typedef struct t$pctr$LayerSetting{
  //? 변수 개수.
  uint64_t count;
  //? 변수 범위 배열.
  uint64_t* ranges;
  //? 가중치 초기화.
  int64_t weight_init;
} t$pctr$LayerSetting;
//? 설정된 레이어의 정보.
typedef struct t$pctr$LayerProp{
  //? 가중치 크기.
  t$pctr$WeightSize$enum weight_size;
  //? 노드 개수.
  uint64_t node_count;
  //? 가중치 초기값.
  int64_t weight_init;
} t$pctr$LayerProp;
typedef struct t$pctr$LayersProp{
  uint64_t prop_count;
  t$pctr$LayerProp* layer_props;
} t$pctr$LayersProp;
//? 모델을 사용할 때 모델의 입출력에 대한 정보.
typedef struct t$pctr$ModelSet{
  //? 입력 레이어 정보.
  t$pctr$LayerSetting input_info;
  //? 중간 레이어 개수.
  uint64_t middle_count;
  //? 중간 레이어 설정.
  t$pctr$LayerProp* middle_infos;
  //? 출력 레이어 정보.
  t$pctr$LayerSetting output_info;
} t$pctr$ModelSet;
typedef struct t$pctr$ModelStats{
  //? 전체 학습(fit()) 시도 횟수.
  uint64_t fit_count;
  //? 세부 학습 시도 횟수.
  uint64_t learn_cycle_count;
  //? 학습 중 가중치 업데이트 횟수.
  uint64_t weight_tune_count;
} t$pctr$ModelStats;
typedef struct t$pctr$Model{
  //? 모델 설정.
  t$pctr$ModelSet setting;
  //? 모델 통계.
  t$pctr$ModelStats stats;

  t$pctr$Layer input_layer;
  t$pctr$Layers middle_layer;
  ///////////////////////////// 출력이 2개 이상인 경우도 만들어질 수 있다.
  t$pctr$Layers out_layer;
} t$pctr$Model;


// --[[ function ]]

// --[ 디버깅 ]

// -- 조건부 화이트리스트 디버깅.
#define CODE_DEBUG 1  // 1:on, 0:off
uint8_t w( const int value )
{
  static uint64_t count = 0;
  static uint64_t exception_count = 0;
  count++;
  if ( CODE_DEBUG && !value )
  {
    exception_count++;
    printf("\n\n[DEBUG][count:%llu, exception_count:%llu][Whitelist Exception!]\n\n", count, exception_count);
  }
  return( value );
}

// -- 조건부 블랙리스트 디버깅.
uint8_t b( const int value )
{
  static uint64_t count = 0;
  static uint64_t exception_count = 0;
  count++;
  if ( CODE_DEBUG && value )
  {
    exception_count++;
    printf("\n\n[DEBUG][count:%llu, exception_count:%llu][Blacklist Exception!]\n\n", count, exception_count);
  }
  return( value );
}

// --[ etc ]

// -- 입력 범위에 따른 가중치 변수 크기.
t$pctr$WeightSize$enum f$pctr$_rangetosize( const uint64_t range )
{
  t$pctr$WeightSize$enum result = e$pctr$WeightSize$bit_null;
  if (b( range<=0 ));
  else if ( range<=((uint8_t)~0/2) ) result = e$pctr$WeightSize$bit_8;
  else if ( range<=((uint16_t)~0/2) ) result = e$pctr$WeightSize$bit_16;
  else if ( range<=((uint32_t)~0/2) ) result = e$pctr$WeightSize$bit_32;
  else if ( range<=((uint64_t)~0/2) ) result = e$pctr$WeightSize$bit_64;
  return( result );
}

// -- raw_node to 1byte node - 1바이트.
int8_t* f$pctr$_rawto1byte( t$pctr$node*const raw_node, const uint64_t index )
{
  return( index+(int8_t*)raw_node );
}

// -- raw_node to 2byte node - 2바이트.
int16_t* f$pctr$_rawto2byte( t$pctr$node*const raw_node, const uint64_t index )
{
  return( index+(int16_t*)raw_node );
}

// -- raw_node to 4byte node - 4바이트.
int32_t* f$pctr$_rawto4byte( t$pctr$node*const raw_node, const uint64_t index )
{
  return( index+(int32_t*)raw_node );
}

// -- raw_node to 8byte node - 8바이트.
int64_t* f$pctr$_rawto8byte( t$pctr$node*const raw_node, const uint64_t index )
{
  return( index+(int64_t*)raw_node );
}


// --[ 검사, 예외처리 ]

// -- WeightSize::Size 검사
uint8_t f$pctr$WeightSize$except( const t$pctr$WeightSize$enum size )
{
  switch ( size )
  {
    case e$pctr$WeightSize$bit_8:
    case e$pctr$WeightSize$bit_16:
    case e$pctr$WeightSize$bit_32:
    case e$pctr$WeightSize$bit_64:return( 1 );

    case e$pctr$WeightSize$bit_null:
    default:return( 0 );
  }
}

// -- 레이어 검사.
uint8_t f$pctr$Layer$except( const t$pctr$Layer layer )
{
  if ( w(layer.raw_node!=0) )
  if ( w(layer.node_count!=0) )
  if ( w(layer.weight_size!=0) )
  return( 1 );
  return( 0 );
}

// -- 레이어 배열 검사.
uint8_t f$pctr$Layers$except( const t$pctr$Layers layers )
{
  if (w( layers.layer_count!=0 ))
    if (w( layers.layers!=0 ))
      for ( uint64_t i=0; i<layers.layer_count; i++ )
        if (w( f$pctr$Layer$except(layers.layers[i]) ));
        else return( 0 );
      return( 1 );
  return( 0 );
}

// -- LayerSetting 검사.
uint8_t f$pctr$LayerSetting$except( const t$pctr$LayerSetting set )
{
  if (w( set.count>=1 ))
  if (w( set.ranges!=0 ))
  return( 1 );
  return( 0 );
}

// -- LayerProp 검사.
uint8_t f$pctr$LayerProp$except( const t$pctr$LayerProp prop )
{
  if (w( f$pctr$WeightSize$except(prop.weight_size) ))
  if (w( prop.node_count>=1 ))
  return( 1 );
  return( 0 );
}

// -- LayersProp 검사.
uint8_t f$pctr$LayersProp$except( const t$pctr$LayersProp props )
{
  if (w( props.prop_count>=1 ))
  if (w( props.layer_props!=0 ))
  {
    for ( uint64_t i=0; i<props.prop_count; i++ )
      if (b( f$pctr$LayerProp$except(props.layer_props[i])==0 )) return( 0 );
    return( 1 );
  }
  return( 0 );
}


// --[ 정적 초기 생성자 ]

// -- 레이어 생성자.
t$pctr$Layer f$pctr$structLayer()
{
  return(
    (t$pctr$Layer)
    {
      .weight_size=0,
      .node_count=0,
      .raw_node=0,
    }
  );
}

// -- 레이어 배열 생성자.
t$pctr$Layers f$pctr$structLayers()
{
  return(
    (t$pctr$Layers)
    {
      .layer_count=0,
      .layers=0,
    }
  );
}

// -- 레이어 설정 생성자.
t$pctr$LayerSetting f$pctr$structLayerSetting()
{
  return(
    (t$pctr$LayerSetting)
    {
      .count=0,
      .ranges=0,
      .weight_init=0,
    }
  );
}

// -- 설정된 레이어의 정보 생성자.
t$pctr$LayerProp f$pctr$structLayerProp()
{
  return(
    (t$pctr$LayerProp)
    {
      .weight_size=e$pctr$WeightSize$bit_null,
      .node_count=0,
      .weight_init=0,
    }
  );
}

// -- 설정된 레이어의 정보 배열 생성자.
t$pctr$LayersProp f$pctr$structLayersProp()
{
  return(
    (t$pctr$LayersProp)
    {
      .prop_count=0,
      .layer_props=0,
    }
  );
}

// -- 모델 설정 생성자.
t$pctr$ModelSet f$pctr$structModelSet()
{
  return(
    (t$pctr$ModelSet)
    {
      .input_info=f$pctr$structLayerSetting(),
      .middle_count=0,
      .middle_infos=0,
      .output_info=f$pctr$structLayerSetting(),
    }
  );
}

// -- 모델 통계 생성자.
t$pctr$ModelStats f$pctr$structModelStats()
{
  return(
    (t$pctr$ModelStats)
    {
      .fit_count=0,
      .learn_cycle_count=0,
      .weight_tune_count=0,
    }
  );
}

// -- 모델 생성자.
t$pctr$Model f$pctr$structModel()
{
  return(
    (t$pctr$Model)
    {
      .setting=f$pctr$structModelSet(),
      .stats=f$pctr$structModelStats(),
      .input_layer=f$pctr$structLayer(),
      .middle_layer=f$pctr$structLayers(),
      .out_layer=f$pctr$structLayers(),
    }
  );
}


// --[ 동적 객체 메모리 해제 ]

// -- 레이어 해제.
t$pctr$Layer f$pctr$Layer$release( const t$pctr$Layer layer )
{
  free(layer.raw_node);
  return( f$pctr$structLayer() );
}

// -- 레이어 배열 해제.
//? 예외 발생시 layers 반환.
t$pctr$Layers f$pctr$Layers$release( const t$pctr$Layers layers )
{
  if (w( f$pctr$Layers$except(layers) ))
  goto KEEP;
  goto SKIP;
  KEEP:;

  for ( uint64_t i=0; i<layers.layer_count; i++ ) f$pctr$Layer$release(layers.layers[i]);
  free(layers.layers);

  return( f$pctr$structLayers() );
  SKIP:return( layers );
}

// -- 레이어 속성 배열 해제.
t$pctr$LayersProp f$pctr$LayersProp$release( const t$pctr$LayersProp props )
{
  if (w( f$pctr$LayersProp$except(props) ))
  goto KEEP;
  goto SKIP;
  KEEP:;

  free(props.layer_props);

  return( f$pctr$structLayersProp() );
  SKIP:return( props );
}

// -- 모델 메모리 해제.
t$pctr$Model f$pctr$Model$release( const t$pctr$Model model )
{
  f$pctr$Layer$release(model.input_layer);
  f$pctr$Layers$release(model.middle_layer);
  f$pctr$Layers$release(model.out_layer);
  return( f$pctr$structModel() );
}


// --[ 동적 생성자 ]

// -- 레이어 생성자.
t$pctr$Layer f$pctr$_newLayer( const t$pctr$LayerProp prop )
{
  t$pctr$Layer result = f$pctr$structLayer();
  uint64_t i = 0;
  if (w( f$pctr$LayerProp$except(prop) ))
  goto KEEP;
  goto SKIP;
  KEEP:;

  //? 메모리 할당.
  result.raw_node = (t$pctr$node*)malloc(prop.weight_size*prop.node_count);
  if ( b(result.raw_node==0) )
    goto SKIP;
  switch ( prop.weight_size )
  {
    case e$pctr$WeightSize$bit_8:
    for ( i=0; i<prop.node_count; i++ ) *f$pctr$_rawto1byte(result.raw_node, i) = (int8_t)prop.weight_init;
    break;
    case e$pctr$WeightSize$bit_16:
    for ( i=0; i<prop.node_count; i++ ) *f$pctr$_rawto2byte(result.raw_node, i) = (int16_t)prop.weight_init;
    break;
    case e$pctr$WeightSize$bit_32:
    for ( i=0; i<prop.node_count; i++ ) *f$pctr$_rawto4byte(result.raw_node, i) = (int32_t)prop.weight_init;
    break;
    case e$pctr$WeightSize$bit_64:
    for ( i=0; i<prop.node_count; i++ ) *f$pctr$_rawto8byte(result.raw_node, i) = (int64_t)prop.weight_init;
    break;
    default:;
  }
  result.weight_size = prop.weight_size;
  result.node_count = prop.node_count;

  SKIP:
  return( result );
}

// -- 레이어 배열 생성자.
t$pctr$Layers f$pctr$_newLayers( const t$pctr$LayersProp props )
{
  t$pctr$Layers result = f$pctr$structLayers();
  uint64_t i = 0;
  if (w( f$pctr$LayersProp$except(props) ))
  goto KEEP;
  goto SKIP;
  KEEP:;

  result.layers = (t$pctr$Layer*)malloc(sizeof(t$pctr$Layer)*props.prop_count);
  if (b( result.layers==0 )) goto SKIP;

  for ( i=0; i<props.prop_count; i++ )
  {
    result.layers[i] = f$pctr$_newLayer(props.layer_props[i]);
    if (b( f$pctr$Layer$except(result.layers[i])==0 ))
    {
      for ( uint64_t c=0; c<=i; c++ )
      {
        result.layers[c] = f$pctr$Layer$release(result.layers[c]);
      }
      free(result.layers);
      goto SKIP;
    }
  }

  result.layer_count = props.prop_count;

  return( result );
  SKIP:return( f$pctr$structLayers() );
}

// -- 레이어 설정 1개만 사용하는 레이어 배열 생성자.
t$pctr$Layers f$pctr$_newLayersOneProp( const uint64_t layer_count, const t$pctr$LayerProp prop )
{
  t$pctr$Layers result = f$pctr$structLayers();
  uint64_t i = 0;
  if (w( f$pctr$LayerProp$except(prop) ))
  if (w( layer_count>=1 ))
  goto KEEP;
  goto SKIP;
  KEEP:;

  result.layers = (t$pctr$Layer*)malloc(sizeof(t$pctr$Layer)*layer_count);
  if (b( result.layers==0 )) goto SKIP;
  result.layer_count = layer_count;

  for ( i=0; i<layer_count; i++ )
  {
    result.layers[i] = f$pctr$_newLayer(prop);
    //////////////////////////////// 성공적으로 생성되었는지 예외처리를 해야하지만 귀찮으며 발생 가능성이 적으므로.......
  }

  SKIP:
  return( result );
}

// -- 레이어 속성 생성자.
t$pctr$LayerProp f$pctr$_newLayerProp( const t$pctr$WeightSize$enum weight_size, const uint64_t node_count, const int64_t weight_init )
{
  return(
    (t$pctr$LayerProp)
    {
      .weight_size=weight_size,
      .node_count=node_count,
      .weight_init=weight_init,
    }
  );
}

// -- 레이어 속성 배열 생성자.
t$pctr$LayersProp f$pctr$_newLayersProp( const uint64_t layer_prop_count, const t$pctr$LayerProp*const Layer_props )
{
  t$pctr$LayersProp result = f$pctr$structLayersProp();
  uint64_t i = 0;
  if (w( Layer_props!=0 ))
  if (w( layer_prop_count>=1 ))
  goto KEEP;
  goto SKIP;
  KEEP:;

  result.layer_props = (t$pctr$LayerProp*)malloc(sizeof(t$pctr$LayerProp)*layer_prop_count);
  if (b( result.layer_props == 0 )) goto SKIP;
  result.prop_count = layer_prop_count;

  for ( i=0; i<layer_prop_count; i++ )
    result.layer_props[i] = Layer_props[i];

  SKIP:
  return( result );
}

// -- 모델 생성자.
t$pctr$Model f$pctr$_newModel(
  //? 입력 정보.
  const uint64_t input_count, const uint64_t*const input_ranges,
  //? 중간 레이어 속성.
  const uint64_t middle_layer_node_count, const uint64_t middle_layer_count,
  //? 출력 정보.
  const uint64_t output_range,
  //? 모델 설정.
  const int64_t weight_init
)
{
  t$pctr$Model result = f$pctr$structModel();
  uint64_t i = 0;
  t$pctr$WeightSize$enum w_size = e$pctr$WeightSize$bit_null;
  uint64_t total_case = 1;
  t$pctr$LayerProp prop = f$pctr$structLayerProp();
  if (w( input_count>=1 ))
  if (w( input_ranges!=0 ))
  if (w( output_range>=2 ))
  //? middle_layer_count가 1 이상인 경우 middle_layer_node_count는 반드시 1 이상이여야 한다.
  if (w( middle_layer_count>=1 && middle_layer_node_count>=1 || middle_layer_count==0 ))
  goto KEEP;
  goto SKIP;
  KEEP:;

  //? 가중치 변수의 크기 구하기.
  w_size = f$pctr$_rangetosize(output_range);

  //? 입력 레이어의 노드 수 구하기.
  for ( i=0; i<input_count; i++ ) total_case *= input_ranges[i];

  prop = f$pctr$_newLayerProp(w_size, total_case, weight_init);
  result.input_layer = f$pctr$_newLayer(prop);

  prop = f$pctr$_newLayerProp(w_size, middle_layer_node_count, weight_init);
  result.middle_layer = f$pctr$_newLayersOneProp(middle_layer_count, prop);

  prop = f$pctr$_newLayerProp(w_size, output_range, weight_init);
  result.out_layer = f$pctr$_newLayers(f$pctr$_newLayersProp(1, &prop));
  
  SKIP:
  return( result );
}
