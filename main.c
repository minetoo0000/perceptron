#include "./Perceptron-1.h"
#include "./dataset/train_set.c"
#include <time.h>


int XXmain( int argc, char* argv[] )
{
  clock_t start_time = 0;
  clock_t end_time = 0;
  int64_t result = 0;

  printf("\n시작.");
  start_time = clock();


  uint64_t input_range[] = { 14 };
  t$pctr$RawData input_data = f$pctr$newRawData(e$pctr$Size$bit_8, 6);
  f$pctr$RawData$set(input_data, 0, 1);
  f$pctr$RawData$set(input_data, 1, 4);
  f$pctr$RawData$set(input_data, 2, 8);
  f$pctr$RawData$set(input_data, 3, 6);
  f$pctr$RawData$set(input_data, 4, 10);
  f$pctr$RawData$set(input_data, 5, 13);
  t$pctr$RawData target_data = f$pctr$newRawData(e$pctr$Size$bit_8, 6);
  f$pctr$RawData$set(target_data, 0, 0);
  f$pctr$RawData$set(target_data, 1, 0);
  f$pctr$RawData$set(target_data, 2, 0);
  f$pctr$RawData$set(target_data, 3, 1);
  f$pctr$RawData$set(target_data, 4, 2);
  f$pctr$RawData$set(target_data, 5, 2);
  t$pctr$Model model = f$pctr$_newModel(
    1, input_range,
    0,0,
    4,
    0
  );
  f$pctr$Model$_fit(model, input_data, target_data, 1, 0);

  end_time = clock();
  printf("\n총 실행에 걸린 시간 : %ldms (%.3fs)", end_time-start_time, (end_time-start_time)/1000.0);

  for ( int i=0; i<=13; i++ )
  {
    result = f$pctr$Model$calc(model, i);
    printf("\n 모델 연산 | in:%d, out:%lld", i, result);
  }

  model = f$pctr$Model$release(model);

  getchar();
  return( 0 );
}



// -- 8*8 도형 이미지 분류 테스트.
//? 레이블 당 8바이트.
//? 사각형 : 0, 삼각형 : 1
int main()
{
  // -- 학습 데이터 준비.
  const uint64_t v$dataset$train_input[] = {
		*(const uint64_t*)v$dataset$train_datasquare,
		*(const uint64_t*)v$dataset$train_datasquare__2_,
		*(const uint64_t*)v$dataset$train_datasquare__3_,
		*(const uint64_t*)v$dataset$train_datasquare__4_,
		*(const uint64_t*)v$dataset$train_datasquare__5_,
		*(const uint64_t*)v$dataset$train_datasquare__6_,
		*(const uint64_t*)v$dataset$train_datasquare__7_,
		*(const uint64_t*)v$dataset$train_datasquare__8_,
		*(const uint64_t*)v$dataset$train_datasquare__9_,
		*(const uint64_t*)v$dataset$train_datasquare__10_,
		*(const uint64_t*)v$dataset$train_datatriangle,
		*(const uint64_t*)v$dataset$train_datatriangle__2_,
		*(const uint64_t*)v$dataset$train_datatriangle__3_,
		*(const uint64_t*)v$dataset$train_datatriangle__4_,
		*(const uint64_t*)v$dataset$train_datatriangle__5_,
		*(const uint64_t*)v$dataset$train_datatriangle__6_,
		*(const uint64_t*)v$dataset$train_datatriangle__8_,
		*(const uint64_t*)v$dataset$train_datatriangle__10_,
    *(const uint64_t*)v$dataset$train_datatriangle__11_,
    *(const uint64_t*)v$dataset$train_datatriangle__12_,
	};
  t$pctr$RawData input_data = f$pctr$newRawData(e$pctr$Size$bit_16, v$dataset$train_data_arr_len);
  uint64_t max_data = 0;
  uint64_t min_data = 0ULL-1;

  //? 최소 최대값 구하기.
  for ( uint64_t i=0; i<v$dataset$train_data_arr_len; i++ )
  {
    if ( max_data<v$dataset$train_input[i] ) max_data = v$dataset$train_input[i];
    if ( min_data>v$dataset$train_input[i] ) min_data = v$dataset$train_input[i];
  }
  printf("\n 최소 : %llu, 최대 : %llu, 차 : %llu", min_data, max_data, max_data-min_data);

  //? 범위 변환.
  for ( uint64_t i=0; i<v$dataset$train_data_arr_len; i++ )
    f$pctr$RawData$set(input_data, i, f$pctr$convRange(min_data, max_data, v$dataset$train_input[i], 0, (1ULL<<15)-1));

  max_data = 0;
  min_data = 0ull-1;
  for ( uint64_t i=0; i<v$dataset$train_data_arr_len; i++ )
  {
    if ( max_data<(uint64_t)f$pctr$RawData$get(input_data, i) ) max_data = (uint64_t)f$pctr$RawData$get(input_data, i);
    if ( min_data>(uint64_t)f$pctr$RawData$get(input_data, i) ) min_data = (uint64_t)f$pctr$RawData$get(input_data, i);
  }
  printf("\n 최소 : %llu, 최대 : %llu", min_data, max_data);

  const uint8_t v$dataset$train_target[] = {
    0,0,0,0,0,0,0,0,0,0,
		1,1,1,1,1,1,1,1,1,1,
	};
  t$pctr$RawData target_data = f$pctr$newRawData(e$pctr$Size$bit_8, v$dataset$train_data_arr_len);
  for ( uint64_t i=0; i<v$dataset$train_data_arr_len; i++ )
    f$pctr$RawData$set(target_data, i, v$dataset$train_target[i]);
  printf("\n 데이터 준비 됨.");

  for ( uint64_t i=0; i<v$dataset$train_data_arr_len; i++ )
  {
    printf("\n input : %llu, target : %d", (uint64_t)f$pctr$RawData$get(input_data, i), (int)f$pctr$RawData$get(target_data, i));
    // printf("\n input : %llu, target : %d", v$dataset$train_input[i], (int)v$dataset$train_target[i]);
  }
  

  // -- 모델 생성.
  t$pctr$Model model = f$pctr$_newModel(
    1, (uint64_t[]){ (1ull<<15)+1 },
    100,1,
    3,
    0
  );
  printf("\n 모델 생성 됨.");


  // -- 모델 학습.
  //? 추가로 시간 측정.
  clock_t start_time = clock();
  f$pctr$Model$_fit(model, input_data, target_data, 1, 0);
  clock_t end_time = clock();
  printf("\n 학습 완료 됨.");


  // -- 걸린 시간 측정.
  printf("\n 학습에 걸린 시간 : %ldms", end_time-start_time);

  
  getchar();
  return( 0 );
}
