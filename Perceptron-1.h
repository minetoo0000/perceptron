
// --[[ include ]]
#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

// --[[ interface ]]

typedef uint8_t t$pctr$node;
typedef enum t$pctr$Size$enum{
  e$pctr$Size$bit_null=0,
  e$pctr$Size$bit_8=1,
  e$pctr$Size$bit_16=2,
  e$pctr$Size$bit_32=4,
  e$pctr$Size$bit_64=8,
} t$pctr$Size$enum;
typedef struct t$pctr$Size{
  const t$pctr$Size$enum bit_null;
  const t$pctr$Size$enum bit_8;
  const t$pctr$Size$enum bit_16;
  const t$pctr$Size$enum bit_32;
  const t$pctr$Size$enum bit_64;
} t$pctr$Size;
const t$pctr$Size e$pctr$Size = {
  .bit_null=e$pctr$Size$bit_null,
  .bit_8=e$pctr$Size$bit_8,
  .bit_16=e$pctr$Size$bit_16,
  .bit_32=e$pctr$Size$bit_32,
  .bit_64=e$pctr$Size$bit_64,
};
typedef struct t$pctr$RawData{
  //? 배열의 원소 크기.
  t$pctr$Size$enum size;
  //? 배열의 길이.
  uint64_t raw_length;
  //? 배열 raw 데이터.
  uint8_t* raw;
} t$pctr$RawData;
typedef struct t$pctr$Layer{
  t$pctr$RawData raw_data;
} t$pctr$Layer;
typedef struct t$pctr$Layers{
  uint64_t layer_count;
  t$pctr$Layer* layers;
} t$pctr$Layers;
typedef struct t$pctr$LayerSetting{
  //? 변수 개수.
  uint64_t count;
  //? 변수 범위 배열.
  uint64_t* ranges;
  //? 가중치 초기화.
  // int64_t weight_init;
} t$pctr$LayerSetting;
//? 설정된 레이어의 정보.
/////////////////////////////////////
typedef struct t$pctr$LayerProp{
  //? 가중치 크기.
  t$pctr$Size$enum weight_size;
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
  // //? 중간 레이어 개수.
  // uint64_t middle_count;
  // //? 중간 레이어 설정.
  // t$pctr$LayerProp* middle_infos;
  //? 출력 레이어 정보.
  t$pctr$LayerSetting output_info;
} t$pctr$ModelSet;
typedef struct t$pctr$ModelStats{
  //? 가중치 업데이트 횟수.
  uint64_t weight_update_count;
  //? 세부 학습 시도 횟수.
  uint64_t learn_cycle_count;
} t$pctr$ModelStats;
typedef struct t$pctr$Model{
  //? 모델 설정.
  t$pctr$ModelSet setting;
  //? 모델 통계.
  t$pctr$ModelStats stats;

  t$pctr$Layer input_layer;
  t$pctr$Layers middle_layer;
  t$pctr$Layers out_layer;
} t$pctr$Model;
typedef struct t$pctr$CalcResult{
  //? 계산 결과.
  int64_t result;
  //? 활성화된 노드의 가장 마지막 인덱스.
  uint64_t checked_index;
} t$pctr$CalcResult;
typedef struct t$pctr$CalcResults{
  uint64_t count;
  t$pctr$CalcResult* calc_results;
} t$pctr$CalcResults;


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

// -- malloc 디버깅.
void* malloc_debug( size_t size )
{
  static int count = 0;
  void* result = 0;
  result = malloc(size);
  count++;
  printf("\ndebug malloc:%p, %llu, count:%d", result, size, count);
  
  return( result );
}
// #define malloc malloc_debug

// -- free 디버깅.
void free_debug( void* mem )
{
  static int count = 0;
  count++;
  printf("\ndebug free:%p, count:%d", mem, count);
  free(mem);
}
// #define free free_debug


// --[ etc ]
// -- 입력 범위에 따른 가중치 변수 크기.
t$pctr$Size$enum f$pctr$_rangetosize( const uint64_t range )
{
  t$pctr$Size$enum result = e$pctr$Size$bit_null;
  if (b( range<=0 ));
  else if ( range<=((uint8_t)~0/2) ) result = e$pctr$Size$bit_8;
  else if ( range<=((uint16_t)~0/2) ) result = e$pctr$Size$bit_16;
  else if ( range<=((uint32_t)~0/2) ) result = e$pctr$Size$bit_32;
  else if ( range<=((uint64_t)~0/2) ) result = e$pctr$Size$bit_64;
  return( result );
}

// -- raw_node to 1byte - 1바이트.
int8_t* f$pctr$_rawto1byte( uint8_t*const raw, const uint64_t index )
{
  return( index+(int8_t*)raw );
}

// -- raw to 2byte - 2바이트.
int16_t* f$pctr$_rawto2byte( uint8_t*const raw, const uint64_t index )
{
  return( index+(int16_t*)raw );
}

// -- raw to 4byte - 4바이트.
int32_t* f$pctr$_rawto4byte( uint8_t*const raw, const uint64_t index )
{
  return( index+(int32_t*)raw );
}

// -- raw to 8byte - 8바이트.
int64_t* f$pctr$_rawto8byte( uint8_t*const raw, const uint64_t index )
{
  return( index+(int64_t*)raw );
}

// -- 노드 위치 계산.
//? 반환값은 num이지만 범위가 0 ~ max_num 으로 제한된다.
//? num이 범위를 초과하는 경우 반환값은 제한이 걸리게 된 최소값(0) 또는 최대값(max_num)이 된다.
uint64_t f$pctr$_indexcutter( const uint64_t max_num, const int64_t num )
{
  if ( num<0 ) return( 0 );
  else if ( num<max_num ) return( num );
  else return( max_num );
}


// --[ 검사, 예외처리 ]
// -- Size 검사
uint8_t f$pctr$Size$except( const t$pctr$Size$enum size )
{
  switch ( size )
  {
    case e$pctr$Size$bit_8:
    case e$pctr$Size$bit_16:
    case e$pctr$Size$bit_32:
    case e$pctr$Size$bit_64:return( 1 );

    case e$pctr$Size$bit_null:
    default:return( 0 );
  }
}

// -- RawData 검사.
uint8_t f$pctr$RawData$except( const t$pctr$RawData raw_data )
{
  if (w( f$pctr$Size$except(raw_data.size) ))
  if (w( raw_data.raw_length>=1 ))
  if (w( raw_data.raw!=0 ))
  return( 1 );
  return( 0 );
}

// -- 레이어 배열 검사.
uint8_t f$pctr$Layers$except( const t$pctr$Layers layers )
{
  if (w( layers.layer_count>=1 ))
    if (w( layers.layers!=0 ))
    {
      for ( uint64_t i=0; i<layers.layer_count; i++ )
        if (w( f$pctr$RawData$except(layers.layers[i].raw_data) ));
        else return( 0 );
      return( 1 );
    }
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
  if (w( f$pctr$Size$except(prop.weight_size) ))
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

// -- ModelSet 검사.
uint8_t f$pctr$ModelSet$except( const t$pctr$ModelSet model_setting )
{
  if (w( f$pctr$LayerSetting$except(model_setting.input_info) ))
  if (w( f$pctr$LayerSetting$except(model_setting.output_info) ))
  return( 1 );
  return( 0 );
}

// -- Model 검사.
uint8_t f$pctr$Model$except( const t$pctr$Model model )
{
  if (w( f$pctr$ModelSet$except(model.setting) ))
  if (w( f$pctr$RawData$except(model.input_layer.raw_data) ))
  if (w( f$pctr$Layers$except(model.out_layer) ))
  return( 1 );
  return( 0 );
}

// -- CalcResult 검사.
uint8_t f$pctr$CalcResults$except( const t$pctr$CalcResults calc_results )
{
    if (w( calc_results.count>=1 ))
    if (w( calc_results.calc_results!=0 ))
    return( 1 );
    return( 0 );
}


// --[ 정적 초기 생성자 ]
// -- 데이터 생성자.
t$pctr$RawData f$pctr$structRawData()
{
  return(
    (t$pctr$RawData)
    {
      .size=e$pctr$Size$bit_null,
      .raw_length=0,
      .raw=0,
    }
  );
}

// -- 레이어 생성자.
t$pctr$Layer f$pctr$structLayer()
{
  return(
    (t$pctr$Layer)
    {
      .raw_data=f$pctr$structRawData(),
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
      // .weight_init=0,
    }
  );
}

// -- 설정된 레이어의 정보 생성자.
t$pctr$LayerProp f$pctr$structLayerProp()
{
  return(
    (t$pctr$LayerProp)
    {
      .weight_size=e$pctr$Size$bit_null,
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
      // .middle_count=0,
      // .middle_infos=0,
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
      .weight_update_count=0,
      .learn_cycle_count=0,
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

// -- 레이어 계산 결과 생성자.
t$pctr$CalcResult f$pctr$structCalcResult()
{
  return(
    (t$pctr$CalcResult)
    {
      .result=0,
      .checked_index=0,
    }
  );
}

// -- 레이어 계산 결과 배열 생성자.
t$pctr$CalcResults f$pctr$structCalcResults()
{
  return(
    (t$pctr$CalcResults)
    {
      .count=0,
      .calc_results=0,
    }
  );
}


// --[ 동적 객체 메모리 해제 ]
// -- RawData 해제.
t$pctr$RawData f$pctr$RawData$release( const t$pctr$RawData raw_data )
{
  free( raw_data.raw );
  return( f$pctr$structRawData() );
}

// -- 레이어 배열 해제.
//? 예외 발생시 layers 반환.
t$pctr$Layers f$pctr$Layers$release( const t$pctr$Layers layers )
{
  if ( layers.layer_count>=1 )
  if ( layers.layers!=0 )
  goto KEEP;
  goto SKIP;
  KEEP:;

  for ( uint64_t i=0; i<layers.layer_count; i++ )
    layers.layers[i].raw_data = f$pctr$RawData$release(layers.layers[i].raw_data);
  free(layers.layers);

  return( f$pctr$structLayers() );
  SKIP:return( layers );
}

// -- 레이어 속성 배열 해제.
t$pctr$LayersProp f$pctr$LayersProp$release( const t$pctr$LayersProp props )
{
  if (w( props.prop_count>=1 ))
  if (w( props.layer_props!=0 ))
  goto KEEP;
  goto SKIP;
  KEEP:;

  free(props.layer_props);

  return( f$pctr$structLayersProp() );
  SKIP:return( props );
}

// -- CalcResults 메모리 해제.
t$pctr$CalcResults f$pctr$CalcResults$release( const t$pctr$CalcResults calc_results )
{
  free(calc_results.calc_results);
  return( f$pctr$structCalcResults() );
}

// -- ModelSet 메모리 해제.
t$pctr$ModelSet f$pctr$ModelSet$release( const t$pctr$ModelSet model_set )
{
  free(model_set.input_info.ranges);
  free(model_set.output_info.ranges);
  return( f$pctr$structModelSet() );
}

// -- 모델 메모리 해제.
t$pctr$Model f$pctr$Model$release( const t$pctr$Model model )
{
  f$pctr$ModelSet$release(model.setting);
  f$pctr$RawData$release(model.input_layer.raw_data);
  f$pctr$Layers$release(model.middle_layer);
  f$pctr$Layers$release(model.out_layer);
  return( f$pctr$structModel() );
}


// --[ etc 2 ]
// -- raw to auto size - 자동.
int64_t f$pctr$RawData$get( const t$pctr$RawData raw, const uint64_t index )
{
  if (w( f$pctr$RawData$except(raw) ))
  if (w( index>=0 ))
  goto KEEP;
  goto SKIP;
  KEEP:;

  if ( raw.size==e$pctr$Size$bit_8 ) return( (int64_t)*(index+(int8_t*)raw.raw) );
  else if ( raw.size==e$pctr$Size$bit_16 ) return( (int64_t)*(index+(int16_t*)raw.raw) );
  else if ( raw.size==e$pctr$Size$bit_32 ) return( (int64_t)*(index+(int32_t*)raw.raw) );
  //? 마지막 경우 까지도 참이 아니라면 오류임.
  else if (w( raw.size==e$pctr$Size$bit_64 )) return( *(index+(int64_t*)raw.raw) );
  else goto SKIP;

  SKIP:return( 0 );
}

// -- raw - 값 설정.
//? 함수 작업 성공 시 1, 예외처리 시 0을 반환.
uint8_t f$pctr$RawData$set( const t$pctr$RawData raw, const uint64_t index, const int64_t set_value )
{
  if (w( f$pctr$RawData$except(raw) ))
  if (w( index>=0 ))
  if (w( index<raw.raw_length ))
  goto KEEP;
  goto SKIP;
  KEEP:;

  if ( raw.size==e$pctr$Size$bit_8 ) *(index+(int8_t*)raw.raw) = (int8_t)set_value;
  else if ( raw.size==e$pctr$Size$bit_16 ) *(index+(int16_t*)raw.raw) = (int16_t)set_value;
  else if ( raw.size==e$pctr$Size$bit_32 ) *(index+(int32_t*)raw.raw) = (int32_t)set_value;
  //? 마지막 경우 까지도 참이 아니라면 오류임.
  else if (w( raw.size==e$pctr$Size$bit_64 )) *(index+(int64_t*)raw.raw) = set_value;
  else goto SKIP;
  
  return( 1 );
  SKIP:return( 0 );
}

// -- RawData 배열 길이 가져오기.
//? 예외 발생 시 0을 반환함.
uint64_t f$pctr$RawData$len( const t$pctr$RawData raw_data )
{
  uint64_t result = 0;
  if (w( f$pctr$RawData$except(raw_data) ))
  goto KEEP;
  goto SKIP;
  KEEP:;

  result = raw_data.raw_length;

  SKIP:
  return( result );
}


// --[ 동적 생성자 ]
// -- RawData 생성자.
t$pctr$RawData f$pctr$newRawData( const t$pctr$Size$enum size, uint64_t length )
{
  t$pctr$RawData result = f$pctr$structRawData();
  uint64_t i = 0;
  if (w( f$pctr$Size$except(size) ))
  if (w( length>=1 ))
  goto KEEP;
  goto SKIP;
  KEEP:;

  result.raw = (uint8_t*)malloc(sizeof(uint8_t)*size*length);
  if (b( result.raw==0 )) goto SKIP;
  result.size = size;
  result.raw_length = length;

  for ( i=0; i<length; i++ )
  {
    //? 값 초기화. 초기화 중 예외 발생 시 메모리 해제하고 종료.
    if (b( f$pctr$RawData$set(result, i, 0)==0 ))
    {
      free(result.raw);
      goto SKIP;
    }
  }

  return( result );
  SKIP:return( f$pctr$structRawData() );
}

// -- ModelSet 생성자.
//? 동적 메모리 할당함.
t$pctr$ModelSet f$pctr$newModelSet(
  const uint64_t input_count, const uint64_t*const input_ranges,
  const uint64_t output_count, const uint64_t*const output_ranges
)
{
  t$pctr$ModelSet result = f$pctr$structModelSet();
  uint64_t i = 0;
  if (w( input_count>=1 ))
  if (w( output_count>=1 ))
  if (w( input_ranges!=0 ))
  if (w( output_ranges!=0 ))
  goto KEEP;
  goto SKIP;
  KEEP:;

  result.input_info.count = input_count;
  result.input_info.ranges = (uint64_t*)malloc(sizeof(uint64_t)*input_count);
  if (b( result.input_info.ranges==0 ))
    goto SKIP;
  for ( i=0; i<input_count; i++ )
    result.input_info.ranges[i] = input_ranges[i];

  result.output_info.count = output_count;
  result.output_info.ranges = (uint64_t*)malloc(sizeof(uint64_t)*output_count);
  if (b( result.output_info.ranges==0 ))
    goto SKIP;
  for ( i=0; i<output_count; i++ )
    result.output_info.ranges[i] = output_ranges[i];

  return( result );
  SKIP:
  result = f$pctr$ModelSet$release(result);
  return( f$pctr$structModelSet() );
}

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
  result.raw_data = f$pctr$newRawData(prop.weight_size, prop.node_count);
  if (b( f$pctr$RawData$except(result.raw_data)==0 ))
    goto SKIP;
  
  //? 가중치 초기화.
  for ( i=0; i<prop.node_count; i++ )
    f$pctr$RawData$set(result.raw_data, i, prop.weight_init);

  SKIP:
  return( result );
}

// -- 레이어 배열 생성자.
t$pctr$Layers f$pctr$newLayers( const t$pctr$LayersProp props )
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
    if (b( f$pctr$RawData$except(result.layers[i].raw_data)==0 ))
    {
      for ( uint64_t c=0; c<=i; c++ )
      {
        result.layers[c].raw_data = f$pctr$RawData$release(result.layers[c].raw_data);
      }
      free(result.layers);
      goto SKIP;
    }
  }

  result.layer_count = props.prop_count;

  if(0)SKIP: result = f$pctr$structLayers();
  return( result );
}

// -- 레이어 설정 1개만 사용하는 레이어 배열 생성자.
//? 메모리 할당함.
t$pctr$Layers f$pctr$newLayersOneProp( const uint64_t layer_count, const t$pctr$LayerProp prop )
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
t$pctr$LayerProp f$pctr$_newLayerProp( const t$pctr$Size$enum weight_size, const uint64_t node_count, const int64_t weight_init )
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
t$pctr$LayersProp f$pctr$newLayersProp( const uint64_t layer_prop_count, const t$pctr$LayerProp*const Layer_props )
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
  t$pctr$Size$enum w_size = e$pctr$Size$bit_null;
  uint64_t total_case = 1;
  t$pctr$LayerProp prop = f$pctr$structLayerProp();
  t$pctr$LayersProp props = f$pctr$structLayersProp();
  if (w( input_count>=1 ))
  if (w( input_ranges!=0 ))
  if (w( output_range>=2 ))
  //? middle_layer_count가 1 이상인 경우 middle_layer_node_count는 반드시 1 이상이여야 한다.
  if (w( middle_layer_count>=1 && middle_layer_node_count>=1 || middle_layer_count==0 ))
  goto KEEP;
  goto SKIP;
  KEEP:;

  result.setting = f$pctr$newModelSet(input_count, input_ranges, 1, (uint64_t[]){ output_range });
  if (b( f$pctr$ModelSet$except(result.setting)==0 ))
    goto SKIP;
  result.stats = f$pctr$structModelStats();

  //? 가중치 변수의 크기 구하기.
  w_size = f$pctr$_rangetosize(output_range);

  //? 입력 레이어의 노드 수 구하기.
  for ( i=0; i<input_count; i++ ) total_case *= input_ranges[i];

  prop = f$pctr$_newLayerProp(w_size, total_case, weight_init);
  result.input_layer = f$pctr$_newLayer(prop);

  if ( middle_layer_count>=1 )
  {
    prop = f$pctr$_newLayerProp(w_size, middle_layer_node_count, weight_init);
    result.middle_layer = f$pctr$newLayersOneProp(middle_layer_count, prop);
  }
  else result.middle_layer = f$pctr$structLayers();

  prop = f$pctr$_newLayerProp(w_size, output_range, weight_init);
  props = f$pctr$newLayersProp(1, &prop);
  result.out_layer = f$pctr$newLayers(props);
  f$pctr$LayersProp$release(props);
  
  return( result );
  SKIP:
  result = f$pctr$Model$release(result);
  return( f$pctr$structModel() );
}

// -- CalcResults 생성자.
t$pctr$CalcResults f$pctr$newCalcResults( const uint64_t count )
{
  t$pctr$CalcResults result = f$pctr$structCalcResults();
  uint64_t i = 0;
  if (w( count>=1 ))
  goto KEEP;
  goto SKIP;
  KEEP:;

  //? 메모리 할당.
  result.calc_results = (t$pctr$CalcResult*)malloc(sizeof(t$pctr$CalcResult)*count);
  if (b( result.calc_results==0 ))
    goto SKIP;
  result.count = count;

  //? 초기화.
  for ( i=0; i<count; i++ )
    result.calc_results[i] = f$pctr$structCalcResult();

  SKIP:
  return( result );
}


// --[ 모델 연산 ]
// -- 레이어 연산.
t$pctr$CalcResult f$pctr$Layer$calc( const t$pctr$Layer layer, const int64_t x )
{
  t$pctr$CalcResult result = f$pctr$structCalcResult();
  uint64_t i = 0;
  uint64_t calc_range = 0;
  if (w( f$pctr$RawData$except(layer.raw_data) ))
  goto KEEP;
  goto SKIP;
  KEEP:;

  calc_range = f$pctr$_indexcutter(f$pctr$RawData$len(layer.raw_data)-1, x);
  for ( i=0; i<=calc_range; i++ )
    result.result += f$pctr$RawData$get(layer.raw_data, i);

  //? 마지막 활성화 노드의 인덱스 체크.
  result.checked_index = i-1;

  SKIP:
  return( result );
}

// -- 레이어 배열 연산.
//? CalcResults는 메모리 해제가 필요하다.
t$pctr$CalcResults f$pctr$Layers$calc( const t$pctr$Layers layers, const int64_t x )
{
  t$pctr$CalcResults result = f$pctr$structCalcResults();
  uint64_t i = 0;
  if (w( f$pctr$Layers$except(layers) ))
  goto KEEP;
  goto SKIP;
  KEEP:;

  result = f$pctr$newCalcResults(layers.layer_count);
  if (b( f$pctr$CalcResults$except(result)==0 ))
    goto SKIP;

  result.calc_results[0] = f$pctr$Layer$calc(layers.layers[0], x);
  for ( i=1; i<layers.layer_count; i++ )
    result.calc_results[i] = f$pctr$Layer$calc(layers.layers[i], result.calc_results[i-1].result);

  SKIP:
  return( result );
}

// -- 모델 연산.
int64_t f$pctr$Model$calc( const t$pctr$Model model, const int64_t x )
{
  int64_t result = 0;
  t$pctr$CalcResult output = f$pctr$structCalcResult();
  t$pctr$CalcResults outputs = f$pctr$structCalcResults();
  if (w( f$pctr$Model$except(model) ))
  goto KEEP;
  goto SKIP;
  KEEP:;

  output = f$pctr$Layer$calc(model.input_layer, x);
  if ( model.middle_layer.layer_count!=0 )
  {
    outputs = f$pctr$Layers$calc(model.middle_layer, output.result);
    output = outputs.calc_results[outputs.count-1];
    outputs = f$pctr$CalcResults$release(outputs);
  }
  outputs = f$pctr$Layers$calc(model.out_layer, output.result);
  output = outputs.calc_results[outputs.count-1];
  outputs = f$pctr$CalcResults$release(outputs);

  result = output.result;

  SKIP:
  return( result );
}


// --[ 모델 학습 ]
// -- 레이어 가중치 업데이트 함수.
//? 작업 성공 시 1, 예외 발생 시 0을 반환.
//? weight_direction
//? true : 가중치 증가.
//? false : 가중치 감소.
//? learning_rate
//? 가중치 변화량 설정.
uint8_t f$pctr$Layer$_weightUpdate( const t$pctr$Layer layer, const uint8_t weight_direction, const t$pctr$CalcResult calc_result, const int64_t learning_rate )
{
  int64_t get_weight = 0;
  if (w( f$pctr$RawData$except(layer.raw_data) ))
  if (w( calc_result.checked_index<f$pctr$RawData$len(layer.raw_data) ))
  goto KEEP;
  goto SKIP;
  KEEP:;

  //? 가중치 증가.
  if ( weight_direction )
  {
    get_weight = f$pctr$RawData$get(layer.raw_data, calc_result.checked_index);
    f$pctr$RawData$set(layer.raw_data, calc_result.checked_index, get_weight+learning_rate);
  }

  //? 가중치 감소, 연산 결과가 0보다 작아지지 않게 하기.
  else if ( calc_result.result>=1 )
  {
    get_weight = f$pctr$RawData$get(layer.raw_data, calc_result.checked_index);
    f$pctr$RawData$set(layer.raw_data, calc_result.checked_index, get_weight-learning_rate);
  }

  return( 1 );
  SKIP:return( 0 );
}

// -- 모델 학습 함수.
//? 작업 성공 시 1을, 예외 발생 시 0을 반환.
//? allow_error_count : 오답 허용 수.
uint8_t f$pctr$Model$_fit( const t$pctr$Model model, const t$pctr$RawData input_data, const t$pctr$RawData target_data, const int64_t learning_rate, const uint64_t allow_error_count )
{
  int32_t state = -1;
  //? 0:input_layer, 1:middle_layer, 2:out_layer
  int32_t select_layer = 0;
  uint64_t i = 0;
  int64_t input = 0;
  int64_t next_x = 0;
  int64_t target = 0;
  uint64_t model_correct_count = 0;
  t$pctr$CalcResult in_result = f$pctr$structCalcResult();
  t$pctr$CalcResults middle_results = f$pctr$structCalcResults();
  t$pctr$CalcResults out_results = f$pctr$structCalcResults();
  if (w( f$pctr$RawData$except(input_data) ))
  if (w( f$pctr$RawData$except(target_data) ))
  if (w( f$pctr$Model$except(model) ))
  if (w( input_data.raw_length==target_data.raw_length ))
  goto KEEP;
  goto SKIP;
  KEEP:;
  
  while ( 1 )
  {
    // -- 학습 끝내기, 종료.
    if ( state==-3 )
    {
      middle_results = f$pctr$CalcResults$release(middle_results);
      out_results = f$pctr$CalcResults$release(out_results);
      break;
    }

    // -- 다음 입력 값으로 업데이트.
    else if ( state==-2 )
    {
      state = 0;

      //? 다음 입력값 가져오기.
      input = f$pctr$RawData$get(input_data, i);
      next_x = input;

      //? 정답도 가져오기.
      target = f$pctr$RawData$get(target_data, i);

      // -- 입력 / 정답 값 계속 순환.
      i++;
      if ( i>=f$pctr$RawData$len(input_data) )
        i = 0;
    }
    
    // -- 초기화.
    else if ( state==-1 )
    {
      state = -2;
      
      select_layer = 0;
      model_correct_count = 0;
      i = 0;
      input = 0;
      next_x = 0;
      target = 0;
      in_result = f$pctr$structCalcResult();
      //? 메모리 해제, 초기화 겸용.
      middle_results = f$pctr$CalcResults$release(middle_results);
      out_results = f$pctr$CalcResults$release(out_results);
    }

    // -- 모델 입력 작업 초기화.
    else if ( state==0 )
    {
      state++;

      next_x = input;
      select_layer = 0;
      in_result = f$pctr$structCalcResult();
      //? 메모리 해제.
      middle_results = f$pctr$CalcResults$release(middle_results);
      out_results = f$pctr$CalcResults$release(out_results);
    }
    
    // -- 입력 레이어 연산.
    else if ( state==1 )
    {
      state++;
      
      in_result = f$pctr$Layer$calc(model.input_layer, next_x);
      next_x = in_result.result;
    }

    // -- 중간 레이어 연산.
    else if ( state==2 )
    {
      state++;

      //? 중간 레이어가 있는 경우에만.
      if ( model.middle_layer.layer_count>=1 )
      {
        middle_results = f$pctr$Layers$calc(model.middle_layer, next_x);
        //? 마지막으로 레이어 연산한 결과 가져오기.
        next_x = middle_results.calc_results[middle_results.count-1].result;
      }
    }

    // -- 출력 레이어 연산.
    else if ( state==3 )
    {
      state++;

      out_results = f$pctr$Layers$calc(model.out_layer, next_x);
      next_x = out_results.calc_results[out_results.count-1].result;
    }

    // -- 정오답 체크 및 모델 학습 완료 여부 검사.
    //? + 루트 갈림.
    else if ( state==4 )
    {
      // -- 정답이라면 다음 입력으로 넘어가기.
      if ( next_x==target )
      {
        state = -2;
        //? 모델이 연속으로 정답을 맞춘 횟수 업데이트.
        model_correct_count++;
        /////////////////////////
        printf("\n정답 횟수. %d", model_correct_count);



        /////////////////////////////// 레이어 출력 디버깅.
        printf("\n target : %lld, model_out : %lld, input : %lld", target, next_x, input);
        //? 행 출력.
        for ( uint64_t row=0; row<14; row++ )
        {
          //? 열 출력.
          printf("\n%c%-3d", in_result.checked_index==row?'|':' ', (int)f$pctr$RawData$get(model.input_layer.raw_data, row));
          if ( row<4 )
          printf(" %c%-3d", out_results.calc_results[0].checked_index==row?'|':' ', (int)f$pctr$RawData$get(model.out_layer.layers[0].raw_data, row));
        }
        getchar();



      }
      // -- 오답이라면 탐색된 노드 인덱스 체크.
      else
      {
        state++;
        //? 한번이라도 틀리면 연속 정답 초기화.
        model_correct_count = 0;
      }

      // -- 모든 입력에 대해 정답이라면 학습 완료.
      if ( (model_correct_count+allow_error_count)>=f$pctr$RawData$len(target_data) )
      {
        state = -3;

        ////////////////////////////////////// 학습 완료 후가 너무 싱겁다..
        printf("\n모델 학습 완료.");
      }
    }

    // -- 모델 결과에 따른 레이어 가중치 업데이트.
    //? + 루트 갈림.
    else if ( state==5 )
    {
      const uint8_t weight_direction = next_x<target;
      uint8_t success = 0;
      uint64_t loop = 0;

      // -- 입력 레이어 학습.
      if ( select_layer==0 )
      {
        select_layer=1;

        success = f$pctr$Layer$_weightUpdate(model.input_layer, weight_direction, in_result, learning_rate);
        if (b( success==0 )) goto SKIP;
      }
      // -- 중간 레이어 학습.
      else if ( select_layer==1 )
      {
        select_layer=2;

        for ( loop=0; loop<model.middle_layer.layer_count; loop++ )
        {
          success = f$pctr$Layer$_weightUpdate(
            model.middle_layer.layers[loop], weight_direction,
            middle_results.calc_results[loop],
            learning_rate
          );
          if (b( success==0 )) goto SKIP;
        }
      }
      // -- 출력 레이어 학습.
      else if ( select_layer==2 )
      {
        //? 학습 시도 완료. 다음 단계로.
        state++;
        select_layer=0;

        for ( loop=0; loop<model.out_layer.layer_count; loop++ )
        {
          success = f$pctr$Layer$_weightUpdate(
            model.out_layer.layers[loop], weight_direction,
            out_results.calc_results[loop],
            learning_rate
          );
          if (b( success==0 )) goto SKIP;
        }


        /////////////////////////////// 레이어 출력 디버깅.
        printf("\n target : %lld, model_out : %lld, input : %lld, weight : %s", target, next_x, input, weight_direction?"증가":"감소");
        //? 행 출력.
        for ( uint64_t row=0; row<14; row++ )
        {
          //? 열 출력.
          printf("\n%c%-3d", in_result.checked_index==row?'|':' ', (int)f$pctr$RawData$get(model.input_layer.raw_data, row));
          if ( row<4 )
          printf(" %c%-3d", out_results.calc_results[0].checked_index==row?'|':' ', (int)f$pctr$RawData$get(model.out_layer.layers[0].raw_data, row));
        }
        getchar();

        
      }
      else select_layer = 0;
    }

    // -- 학습 시도 완료, 오답이 출력되었던 현재 입력으로 다시 모델에 입력.
    else if ( state==6 )
    {
      state = 0;
    }

    // -- 에러.
    else
    {
      goto SKIP;
    }
  }

  return( 1 );
  SKIP:
  middle_results = f$pctr$CalcResults$release(middle_results);
  out_results = f$pctr$CalcResults$release(out_results);
  return( 0 );
}
