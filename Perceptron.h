/**
 * 1번 레이어의 결과가 음수로 나온 경우 2가지 선택지가 있다.
 * 앞으로의 모든 레이어를 건너 뛰고 0을 출력하거나
 * 2번 레이어의 결과를 0으로 하여 3번 부터의 레이어를 연산하여 결과를 출력하거나이다.
*/

/**
 * False Nagative인 경우엔 포인트된 노드를 증가시켜주면 되지만
 * False Positive인 경우는 포인트된 노드를 감소시켜주면 다음 레이어 연산 때 감소된 레이어 기준,
 * 그 이후 레이어에서 감소시켜준 것이 의미가 없어짐.
*/

/**
 * x가 0보다 작아지는 경우 0번 노드가 -1인 경우 해당 레이어의 연산 결과가
 * 음수가 아닌 0이 나온다. x가 작을 수록 음수가 필요했다면 문제가 발생할 수도 있겠다.
*/

/**
 * malloc 종류 및 개수와 free 종류 및 개수가 일치하는지 검사.
 * 다만 newMiddleLayerStaticRange 함수와 같이 중복 종류는 제외하고 검사.
*/

/**
 * 출력 레이어에서 출력 값이 현재 알고리즘으로는 음수가 나올 수 있다.
 * 이 경우 원하는 범위의 값이 나오지 않을 수 있게 된다. 범위를 0과 양수로만 제한할 것인가?
 * 아니면 범위 변환 함수를 사용할 것인가?
*/


// --[[ include ]]
#include <stdio.h>
#include <stdint.h>
#include <malloc.h>


// --[[ interface ]]

// --[ 다중 분류 모델 ]

// -- 최소 단위 노드.
typedef struct t$pctr$Node{
  union bin{
    int16_t bin;
    struct nodes{
      int16_t w1:2;
      int16_t w2:2;
      int16_t w3:2;
      int16_t w4:2;
      int16_t w5:2;
      int16_t w6:2;
      int16_t w7:2;
      int16_t w8:2;
    } nodes;
  } bin;
} t$pctr$Node;
// -- 노드 레이어.
typedef struct t$pctr$Layer{
  t$pctr$Node* nodes;
  uint64_t node_len;
} t$pctr$Layer;
// -- 입력 레이어.
typedef struct t$pctr$InputLayer{
  //? 입력 레이어.
  t$pctr$Layer layer;
  // //? 변수의 범위를 담는 배열.
  // uint64_t* var_ranges;
  // //? 변수의 개수. 즉 ranges 배열의 길이.
  // uint64_t var_count;
} t$pctr$InputLayer;
// -- 중간 레이어.
typedef struct t$pctr$MiddleLayer{
  t$pctr$Layer* layers;
  uint64_t layer_count;
} t$pctr$MiddleLayer;
// -- 출력 레이어.
typedef struct t$pctr$OutLayer{
  t$pctr$Layer layer;
} t$pctr$OutLayer;
// -- 모델.
typedef struct t$pctr$Model{
  t$pctr$InputLayer input_layer;
  t$pctr$MiddleLayer middle_layer;
  t$pctr$OutLayer out_layer;
  //? 총 학습 시도 수.
  uint64_t total_try_learn;
  //? 학습 중, 총 정답 수.
  uint64_t total_try_learn_true;
  //? 학습 중, 총 오답 수.
  uint64_t total_try_learn_false;
} t$pctr$Model;
// -- 레이어 가중치 포인터.
typedef struct t$pctr$WPointer{
  uint64_t node;
  uint8_t w;
} t$pctr$WPointer;

// --[ 데이터 ]

// -- 학습 데이터.
// typedef struct t$pctr$FitData{
  
// } t$pctr$Data;


// --[[ function ]]

// -- 노드 가중치값 설정용 함수.
//? w가 -1, 0, 1 이 외의 값일 경우 0을 반환함.
int8_t f$pctr$_wset( const int8_t w )
{
  int8_t result = 0;
  switch ( w )
  {
    case 1:result+=2;
    case -1:result--;
    default:;
  }
  return( result );
}
// -- 새 노드 복제자.
//? init은 각 노드의 초기값.
//? -1, 0, 1 중에서 초기화 가능. 그 외 값인 경우 0으로 초기화됨.
t$pctr$Node f$pctr$structNode( const int8_t init )
{
  return(
    (t$pctr$Node)
    {
      .bin.nodes = {
        .w1 = f$pctr$_wset(init),
        .w2 = f$pctr$_wset(init),
        .w3 = f$pctr$_wset(init),
        .w4 = f$pctr$_wset(init),
        .w5 = f$pctr$_wset(init),
        .w6 = f$pctr$_wset(init),
        .w7 = f$pctr$_wset(init),
        .w8 = f$pctr$_wset(init),
      },
    }
  );
}
// -- 새 레이어 복제자.
t$pctr$Layer f$pctr$structLayer()
{
  return(
    (t$pctr$Layer)
    {
      .nodes = 0,
      .node_len = 0,
    }
  );
}
// -- 새 입력 레이어 복제자.
t$pctr$InputLayer f$pctr$structInputLayer()
{
  return(
    (t$pctr$InputLayer)
    {
      .layer = f$pctr$structLayer(),
    }
  );
}
// -- 새 중간 레이어 복제자.
t$pctr$MiddleLayer f$pctr$structMiddleLayer()
{
  return(
    (t$pctr$MiddleLayer)
    {
      .layers = 0,
      .layer_count = 0,
    }
  );
}
// -- 새 출력 레이어 복제자.
t$pctr$OutLayer f$pctr$structOutLayer()
{
  return(
    (t$pctr$OutLayer)
    {
      .layer={
        .nodes=0,
        .node_len=0,
      },
      // .bit_ptr=0,
    }
  );
}
// -- 새 모델 복제자.
t$pctr$Model f$pctr$structModel()
{
  return(
    (t$pctr$Model)
    {
      .input_layer=f$pctr$structInputLayer(),
      .middle_layer=f$pctr$structMiddleLayer(),
      .out_layer=f$pctr$structOutLayer(),
      .total_try_learn=0,
      .total_try_learn_true=0,
      .total_try_learn_false=0,
    }
  );
}
// -- 새 레이어 가중치 포인터 제작자.
t$pctr$WPointer f$pctr$structWPointer()
{
  return(
    (t$pctr$WPointer)
    {
      .node = 0,
      .w = 0,
    }
  );
}


// --[[ Node function ]]

// --[ 학습 ]

// -- 가중치 설정 함수.
//? 8 이상의 index는 node를 반환한다.
t$pctr$Node f$pctr$Node$setBit( t$pctr$Node node, const uint8_t index, int8_t set )
{
  set = f$pctr$_wset(set);
  switch ( index )
  {
    case 0:return( node.bin.nodes.w1 = set, node );
    case 1:return( node.bin.nodes.w2 = set, node );
    case 2:return( node.bin.nodes.w3 = set, node );
    case 3:return( node.bin.nodes.w4 = set, node );
    case 4:return( node.bin.nodes.w5 = set, node );
    case 5:return( node.bin.nodes.w6 = set, node );
    case 6:return( node.bin.nodes.w7 = set, node );
    case 7:return( node.bin.nodes.w8 = set, node );
    default:return( node );
  }
}
// -- 노드 계산 함수.
int8_t f$pctr$Node$calc( t$pctr$Node node, const int8_t x )
{
  int8_t result = 0;
  switch ( x )
  {
    case 7:result+=node.bin.nodes.w8;
    case 6:result+=node.bin.nodes.w7;
    case 5:result+=node.bin.nodes.w6;
    case 4:result+=node.bin.nodes.w5;
    case 3:result+=node.bin.nodes.w4;
    case 2:result+=node.bin.nodes.w3;
    case 1:result+=node.bin.nodes.w2;
    case 0:result+=node.bin.nodes.w1;
    default:;
  }
  return( result );
}


// --[[ Layer function ]]

// -- 예외처리 함수.
//? 정상인 경우 1, 아니면 0.
uint8_t f$pctr$Layer$except( const t$pctr$Layer layer )
{
  if ( layer.node_len )
  if ( layer.nodes )
  return( 1 );
  return( 0 );
}
// -- 레이어 배열 예외처리 함수.
uint8_t f$pctr$Layer$excepts( const uint64_t length, const t$pctr$Layer*const layers )
{
  uint8_t result = 0;
  
  if ( length )
  if ( layers )
  goto KEEP;
  goto SKIP;
  KEEP:;

  for ( uint64_t i=length; i; i-- )
    result |= !f$pctr$Layer$except(layers[i]);

  return( result?0:1 );
  SKIP:return( 0 );
}

// --[ 생성자 ]

// -- 새 레이어 생성자.
//? length가 0인 경우 structLayer()가 반환된다.
t$pctr$Layer f$pctr$newLayer( const uint64_t length, const int8_t init )
{
  t$pctr$Layer result = f$pctr$structLayer();
  t$pctr$Node* new_space = 0;
  if ( length )
  goto KEEP;
  goto SKIP;
  KEEP:;

  new_space = (t$pctr$Node*)malloc(sizeof(t$pctr$Node)*length);
  if ( new_space );
  else goto SKIP;

  for ( uint64_t i=0; i<length; i++ )
    new_space[i] = f$pctr$structNode(init);

  result = (t$pctr$Layer){
    .nodes = new_space,
    .node_len = length,
  };

  SKIP:return( result );
}
// -- 새 입력 레이어 생성자.
t$pctr$InputLayer f$pctr$newInputLayer( const uint64_t length, const int8_t init )
{
  t$pctr$InputLayer result = f$pctr$structInputLayer();

  if ( length )
  goto KEEP;
  goto SKIP;
  KEEP:;

  result.layer = f$pctr$newLayer(length, init);

  SKIP:return( result );
}
// -- 새 중간 레이어 생성자.
t$pctr$MiddleLayer f$pctr$newMiddleLayer( const uint64_t layer_count, const uint64_t*const layer_ranges, const int8_t init )
{
  t$pctr$MiddleLayer result = f$pctr$structMiddleLayer();

  if ( layer_count )
  if ( layer_ranges )
  goto KEEP;
  goto SKIP;
  KEEP:;

  // -- layer_ranges 배열의 요소가 0인지 아닌지 검사.
  for ( uint64_t i=0; i<layer_count; i++ )
    if ( layer_ranges[i] == 0 ) goto SKIP;

  result.layers = (t$pctr$Layer*)malloc(sizeof(t$pctr$Layer)*layer_count);
  if ( result.layers == 0 ) goto SKIP;

  for ( uint64_t i=0; i<layer_count; i++ )
    result.layers[i] = f$pctr$newLayer(layer_ranges[i], init);

  result.layer_count = layer_count;
  
  SKIP:return( result );
}
// -- 새 중간 레이어 생성자 + 고정 길이 레이어.
t$pctr$MiddleLayer f$pctr$newMiddleLayerStaticRange( const uint64_t layer_count, uint64_t layer_length, const int8_t init )
{
  t$pctr$MiddleLayer result = f$pctr$structMiddleLayer();

  if ( layer_count )
  if ( layer_length )
  goto KEEP;
  goto SKIP;
  KEEP:;

  result.layers = (t$pctr$Layer*)malloc(sizeof(t$pctr$Layer)*layer_count);
  if ( result.layers == 0 ) goto SKIP;

  for ( uint64_t i=0; i<layer_count; i++ )
    result.layers[i] = f$pctr$newLayer(layer_length, init);

  result.layer_count = layer_count;
  
  SKIP:return( result );
}
// -- 새 출력 레이어 생성자.
t$pctr$OutLayer f$pctr$newOutLayer( const uint64_t length, const int8_t init )
{
  return(
    (t$pctr$OutLayer)
    {
      .layer=f$pctr$newLayer(length, init),
    }
  );
}

// --[ 메모리 해제 ]

// -- 레이어 메모리 할당 해제.
//? layer.node_len이나 layer.nodes가 0이면 layer를 반환.
t$pctr$Layer f$pctr$Layer$releaseLayer( t$pctr$Layer layer )
{
  if ( layer.node_len )
  if ( layer.nodes )
  goto KEEP;
  goto SKIP;
  KEEP:;

  free(layer.nodes);
  layer.nodes = 0;
  layer.node_len = 0;

  SKIP:return( layer );
}
// -- 입력 레이어 메모리 할당 해제.
t$pctr$InputLayer f$pctr$InputLayer$releaseInputLayer( t$pctr$InputLayer layer )
{
  if ( f$pctr$Layer$except(layer.layer) )
  goto KEEP;
  goto SKIP;
  KEEP:;

  layer.layer = f$pctr$Layer$releaseLayer(layer.layer);

  SKIP:return( layer );
}
// -- 중간 레이어 메모리 할당 해제.
t$pctr$MiddleLayer f$pctr$MiddleLayer$releaseMiddleLayer( t$pctr$MiddleLayer layer )
{
  if ( layer.layers )
  if ( layer.layer_count )
  goto KEEP;
  goto SKIP;
  KEEP:;

  for ( uint64_t i=0; i<layer.layer_count; i++ )
  {
    f$pctr$Layer$releaseLayer(layer.layers[i]);
  }

  free(layer.layers);
  layer.layers = 0;
  layer.layer_count = 0;

  SKIP:return( layer );
}
// -- 출력 레이어 메모리 할당 해제.
t$pctr$OutLayer f$pctr$OutLayer$releaseOutLayer( t$pctr$OutLayer layer )
{
  if ( layer.layer.node_len )
  if ( layer.layer.nodes )
  goto KEEP;
  goto SKIP;
  KEEP:;

  layer.layer = f$pctr$Layer$releaseLayer(layer.layer);

  SKIP:return( layer );
}

// --[ 학습 ]

// -- 입력 x에 따른 가중치 포인터 계산 함수.
t$pctr$WPointer f$pctr$Layer$calcWeightPointer( const uint64_t layer_node_len, const int64_t x )
{
  t$pctr$WPointer result = f$pctr$structWPointer();

  if ( layer_node_len )
  goto KEEP;
  goto SKIP;
  KEEP:;

  // -- 무조건 연산되는 노드 개수 - 1 구하기.
  result.node = x / 8;
  // -- 모든 가중치가 연산되지 않는 노드(맨 마지막 1개 제외)의 가중치 개수 구학.
  result.w = x % 8;
  // -- 레이어의 길이보다 x가 더 큰 경우 예외처리.
  if ( result.node > layer_node_len-1 )
  {
    result.node = layer_node_len - 1;
    result.w = 7;
  }

  SKIP:return( result );
}
// -- 레이어 연산 함수.
//? x가 음수인 경우 0을 반환.
int64_t f$pctr$Layer$calc( const t$pctr$Layer layer, const int64_t x )
{
  int64_t result = 0;
  t$pctr$WPointer index = f$pctr$structWPointer();

  if ( f$pctr$Layer$except(layer) )
  goto KEEP;
  goto SKIP;
  KEEP:;

  // -- x 검사.
  if ( x <= -1 ) goto SKIP;
  // -- x에 따른 레이어의 계산할 범위 구하기.
  index = f$pctr$Layer$calcWeightPointer(layer.node_len, x);
  // -- 무조건 전부 연산되는 노드 - 1의 계산.
  for ( uint64_t i=index.node; i; i-- )
  {
    result += f$pctr$Node$calc(layer.nodes[i], 7);
  }
  // -- 남은 연산해야할 8개 이하의 가중치 계산.
  result += f$pctr$Node$calc(layer.nodes[index.node], index.w);

  SKIP:return( result );
}
// -- 입력 레이어 연산.
int64_t f$pctr$InputLayer$calc( const t$pctr$InputLayer layer, const int64_t x )
{
  int64_t result = 0;

  if ( f$pctr$Layer$except(layer.layer) )
  goto KEEP;
  goto SKIP;
  KEEP:;

  // -- 입력 레이어 연산.
  result = f$pctr$Layer$calc(layer.layer, x);

  SKIP:return( result );
}
// -- 중간 레이어 연산.
int64_t f$pctr$MiddleLayer$calc( const t$pctr$MiddleLayer layer, const int64_t x )
{
  int64_t result = 0;
  int64_t layer_x = 0;

  if ( f$pctr$Layer$excepts(layer.layer_count, layer.layers) )
  goto KEEP;
  goto SKIP;
  KEEP:;

  // -- 첫 번째 레이어는 함수로 입력된 x로 연산 결과 구하기.
  layer_x = f$pctr$Layer$calc(layer.layers[0], x);

  // -- 두 번째 레이어부턴 첫 번째 레이어의 연산 결과로 입력.
  for ( uint64_t i=1; i<layer.layer_count; i++ )
    layer_x = f$pctr$Layer$calc(layer.layers[i], layer_x);

  // -- 마지막으로 계산된 레이어 연산 결과가 최종 결과임.
  result = layer_x;

  SKIP:return( result );
}
// -- 출력 레이어 연산.
int64_t f$pctr$OutLayer$calc( const t$pctr$OutLayer layer, const int64_t x )
{
  int64_t result = 0;

  if ( f$pctr$Layer$except(layer.layer) )
  goto KEEP;
  goto SKIP;
  KEEP:;

  result = f$pctr$Layer$calc(layer.layer, x);

  SKIP:return( result );
}


// --[[ Model function ]]

// -- 모델 예외 검사.
uint8_t f$pctr$Model$except( t$pctr$Model model )
{
  if ( f$pctr$Layer$except(model.input_layer.layer) )
  ///////////////////////////////! 중간 레이어는 필수 레이어가 아니므로 객체가 비어있는 상태여도 상관없음.
  // if ( f$pctr$Layer$excepts(model.middle_layer.layer_count, model.middle_layer.layers) )
  if ( f$pctr$Layer$except(model.out_layer.layer) )
  return( 1 );
  return( 0 );
}

// --[ 생성자 ]

// -- 새 모델 생성자.
//? var_count : 모델에 입력으로 넣을 변수의 개수.
//? var_ranges : 입력으로 넣을 각 변수의 범위를 담은 배열.
//? middle_layer_count : 중간 레이어의 개수. 0개도 가능.
//? output_range : 모델의 출력 범위.
//? init : 모델 생성 시 노드의 가중치의 초기값. (-1, 0, 1)
t$pctr$Model f$pctr$newModel( const uint64_t var_count, const uint64_t*const var_ranges, const uint64_t middle_layer_count, const uint64_t output_range, const int8_t init )
{
  t$pctr$Model result = f$pctr$structModel();
  uint64_t total_range = 1;
  uint64_t output_length = 0;
  uint64_t input_length = 0;

  if ( var_count )
  if ( var_ranges )
  if ( output_range )
  goto KEEP;
  goto SKIP;
  KEEP:;

  // -- 입력 레이어 길이 구하기.
  for ( uint64_t i=0; i<var_count; i++ )
    if ( var_ranges[i] ) total_range *= var_ranges[i];

  // -- 출력 레이어의 노드 길이 구하기.
  output_length = (output_range-1) / 8 + 1;
  input_length = (total_range-1) / 8 + 1;

  result.input_layer = f$pctr$newInputLayer(input_length, init);
  result.middle_layer = f$pctr$newMiddleLayerStaticRange(middle_layer_count, output_length, init);
  result.out_layer = f$pctr$newOutLayer(output_length, init);

  SKIP:return( result );
}

// --[ 메모리 해제 ]

// -- 모델 메모리 해제.
t$pctr$Model f$pctr$Model$releaseModel( t$pctr$Model model )
{
  model.input_layer = f$pctr$InputLayer$releaseInputLayer(model.input_layer);
  model.middle_layer = f$pctr$MiddleLayer$releaseMiddleLayer(model.middle_layer);
  model.out_layer = f$pctr$OutLayer$releaseOutLayer(model.out_layer);
  return( model );
}

// --[ 학습 ]

// -- 모델 연산 결과 함수.
//? model : 모델 객체.
//? var_count : 입력 x 변수의 개수. 즉 x 배열의 길이.
//? x : 입력값 배열.
int64_t f$pctr$Model$predict_raw( const t$pctr$Model model, const int64_t x)
{
  int64_t result = 0;
  int64_t layer_x = 0;

  if ( f$pctr$Model$except(model) )
  goto KEEP;
  goto SKIP;
  KEEP:;

  // -- 입력 레이어 연산.
  layer_x = f$pctr$InputLayer$calc(model.input_layer, x);
  // -- 중간 레이어 연산.
  layer_x = f$pctr$MiddleLayer$calc(model.middle_layer, layer_x);
  // -- 출력 레이어 연산.
  layer_x = f$pctr$OutLayer$calc(model.out_layer, layer_x);

  // -- 최종 레이어 연산 결과를 최종 모델 연산 결과로 사용.
  result = layer_x;

  SKIP:return( result );
}
