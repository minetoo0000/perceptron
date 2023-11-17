#include "./Perceptron-1.h"
#include "./dataset/train_set.c"
#include <time.h>


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
    *(const uint64_t*)v$dataset$train_datatriangle__7_,
    *(const uint64_t*)v$dataset$train_datatriangle__8_,
    *(const uint64_t*)v$dataset$train_datatriangle__9_,
    *(const uint64_t*)v$dataset$train_datatriangle__10_,

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


  //? 입력 범위 변환.
  for ( uint64_t i=0; i<v$dataset$train_data_arr_len; i++ )
    f$pctr$RawData$set(input_data, i, f$pctr$convRange(0, max_data, v$dataset$train_input[i], 0, (1ULL<<15)-1));


  //? 변환된 값의 최소 최대값 구하기.
  max_data = 0;
  min_data = 0ull-1;
  for ( uint64_t i=0; i<v$dataset$train_data_arr_len; i++ )
  {
    if ( max_data<(uint64_t)f$pctr$RawData$get(input_data, i) ) max_data = (uint64_t)f$pctr$RawData$get(input_data, i);
    if ( min_data>(uint64_t)f$pctr$RawData$get(input_data, i) ) min_data = (uint64_t)f$pctr$RawData$get(input_data, i);
  }
  printf("\n 최소 : %llu, 최대 : %llu", min_data, max_data);


  //? 두 입력 데이터의 차 중 제일 작은 차 구하기.
  uint64_t min_abs = 0ull-1;
  for ( uint64_t i=0; i<v$dataset$train_data_arr_len-1; i++ )
  {
    uint64_t selected = v$dataset$train_input[i];
    for ( uint64_t j=i+1; j<v$dataset$train_data_arr_len; j++ )
    {
      uint64_t calc = selected>v$dataset$train_input[j]?selected-v$dataset$train_input[j]:v$dataset$train_input[j]-selected;
      // printf("\n 차 값 : %llu", calc);
      if ( min_abs>calc ) min_abs = calc;
      if ( calc==0 ) printf("\n 중복 값 인덱스 : %llu, %llu", i, j);
    }
  }
  printf("\n 최소 차 : %llu", min_abs);


  //? 정답 데이터 준비.
  const uint8_t v$dataset$train_target[] = {
    0,0,0,0,0,0,0,0,0,0,
		1,1,1,1,1,1,1,1,1,1,
	};
  t$pctr$RawData target_data = f$pctr$newRawData(e$pctr$Size$bit_8, v$dataset$train_data_arr_len);
  for ( uint64_t i=0; i<v$dataset$train_data_arr_len; i++ )
    f$pctr$RawData$set(target_data, i, v$dataset$train_target[i]);
  printf("\n 데이터 준비 됨.");


  //? 입력, 정답 데이터 확인.
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
