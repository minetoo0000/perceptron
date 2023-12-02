// #include "./Perceptron-1.h"
// #include "./dataset/img_train_set.c"
// // #include "./dataset/house_train_set.c"
// #include <time.h>


// // -- 8*8 도형 이미지 분류 테스트.
// //? 레이블 당 8바이트.
// //? 사각형 : 0, 삼각형 : 1
// int main()
// {
//   // -- 학습 데이터 준비.
//   printf("\n 학습 데이터 준비중... ");
//   const uint64_t v$dataset$train_input[] = {
    
//     // -- 사각형 데이터.
//     *(const uint64_t*)v$dataset$train_datasquare,
//     *(const uint64_t*)v$dataset$train_datasquare__2_,
//     *(const uint64_t*)v$dataset$train_datasquare__3_,
//     *(const uint64_t*)v$dataset$train_datasquare__4_,
//     *(const uint64_t*)v$dataset$train_datasquare__5_,
//     *(const uint64_t*)v$dataset$train_datasquare__6_,
//     *(const uint64_t*)v$dataset$train_datasquare__7_,
//     *(const uint64_t*)v$dataset$train_datasquare__8_,
//     *(const uint64_t*)v$dataset$train_datasquare__9_,
//     *(const uint64_t*)v$dataset$train_datasquare__10_,


//     // -- 삼각형 데이터.
//     *(const uint64_t*)v$dataset$train_datatriangle,
//     *(const uint64_t*)v$dataset$train_datatriangle__2_,
//     *(const uint64_t*)v$dataset$train_datatriangle__3_,
//     *(const uint64_t*)v$dataset$train_datatriangle__4_,
//     *(const uint64_t*)v$dataset$train_datatriangle__5_,
//     *(const uint64_t*)v$dataset$train_datatriangle__6_,
//     *(const uint64_t*)v$dataset$train_datatriangle__7_,
//     *(const uint64_t*)v$dataset$train_datatriangle__8_,
//     *(const uint64_t*)v$dataset$train_datatriangle__9_,
//     *(const uint64_t*)v$dataset$train_datatriangle__10_,

// 	};
//   uint64_t train_len = 20;
//   t$pctr$RawData input_data = f$pctr$newRawData(e$pctr$Size$bit_16, train_len);
//   uint64_t max_data = 0;
//   uint64_t min_data = 0ULL-1;


//   //? 최소 최대값 구하기.
//   for ( uint64_t i=0; i<train_len; i++ )
//   {
//     if ( max_data<v$dataset$train_input[i] ) max_data = v$dataset$train_input[i];
//     if ( min_data>v$dataset$train_input[i] ) min_data = v$dataset$train_input[i];
//   }
//   printf("\n 최소 : %llu, 최대 : %llu, 차 : %llu", min_data, max_data, max_data-min_data);


//   //? 입력 범위 변환.
//   for ( uint64_t i=0; i<train_len; i++ )
//     f$pctr$RawData$set(input_data, i, f$pctr$convRange(min_data, max_data, v$dataset$train_input[i], 0, (1ULL<<15)-1));


//   //? 변환된 값의 최소 최대값 구하기.
//   max_data = 0;
//   min_data = 0ull-1;
//   for ( uint64_t i=0; i<train_len; i++ )
//   {
//     if ( max_data<(uint64_t)f$pctr$RawData$get(input_data, i) ) max_data = (uint64_t)f$pctr$RawData$get(input_data, i);
//     if ( min_data>(uint64_t)f$pctr$RawData$get(input_data, i) ) min_data = (uint64_t)f$pctr$RawData$get(input_data, i);
//   }
//   printf("\n 최소 : %llu, 최대 : %llu", min_data, max_data);

//   for ( uint64_t i=0; i<f$pctr$RawData$len(input_data); i++ )
//   {
//     printf("\n conv_data : %20llu", f$pctr$RawData$get(input_data, i));
//   }
//   getchar();

//   //? 두 입력 데이터의 차 중 제일 작은 차 구하기.
//   uint64_t min_abs = 0ull-1;
//   for ( uint64_t i=0; i<train_len-1; i++ )
//   {
//     uint64_t selected = f$pctr$RawData$get(input_data, i);
//     for ( uint64_t j=i+1; j<train_len; j++ )
//     {
//       uint64_t calc = selected>f$pctr$RawData$get(input_data, j)?selected-f$pctr$RawData$get(input_data, j):f$pctr$RawData$get(input_data, j)-selected;
//       // printf("\n 차 값 : %llu", calc);
//       if ( min_abs>calc ) min_abs = calc;
//       if ( calc==0 ) printf("\n 중복 값 인덱스 : %llu, %llu", i, j);
//     }
//   }
//   printf("\n 최소 차 : %llu", min_abs);


//   //? 정답 데이터 준비.
//   const uint8_t v$dataset$train_target[] = {
//     0,0,0,0,0,0,0,0,0,0,
// 		1,1,1,1,1,1,1,1,1,1,
// 	};
//   t$pctr$RawData target_data = f$pctr$newRawData(e$pctr$Size$bit_8, train_len);
//   for ( uint64_t i=0; i<train_len; i++ )
//     f$pctr$RawData$set(target_data, i, v$dataset$train_target[i]);
//   printf(" OK");
//   getchar();


//   //? 입력, 정답 데이터 확인.
//   for ( uint64_t i=0; i<train_len; i++ )
//   {
//     // printf("\n input : %llu, target : %d", (uint64_t)f$pctr$RawData$get(input_data, i), (int)f$pctr$RawData$get(target_data, i));
//     // printf("\n input : %llu, target : %d", v$dataset$train_input[i], (int)v$dataset$train_target[i]);
//   }
  

//   // -- 모델 생성.
//   printf("\n 모델 생성중... ");
//   t$pctr$Model model = f$pctr$_newModel(
//     1, (uint64_t[]){ (1ull<<15)+1 },
//     100,1,
//     3,
//     0
//   );
//   printf(" OK");
//   getchar();


//   // -- 모델 학습.
//   //? 추가로 시간 측정.
//   printf("\n 학습 시작... ");
//   double start_time = clock()/(double)CLOCKS_PER_SEC;
//   f$pctr$Model$_fit(model, input_data, target_data, 1, 0);
//   double end_time = clock()/(double)CLOCKS_PER_SEC;
//   printf(" OK");


//   // -- 걸린 시간 측정.
//   printf("\n 학습에 걸린 시간 : %llfs", end_time-start_time);
//   getchar();


//   // -- 테스트 데이터 확인.
//   printf("\n 테스트 데이터로 검증");
//   const uint64_t _test_input[] = {
//     *(const uint64_t*)v$dataset$train_datasquare__11_,
//     *(const uint64_t*)v$dataset$train_datasquare__12_,
//     *(const uint64_t*)v$dataset$train_datasquare__14_,
//     *(const uint64_t*)v$dataset$train_datasquare__15_,
//     *(const uint64_t*)v$dataset$train_datasquare__16_,


//     *(const uint64_t*)v$dataset$train_datatriangle__11_,
//     *(const uint64_t*)v$dataset$train_datatriangle__16_,
//     *(const uint64_t*)v$dataset$train_datatriangle__17_,
//     *(const uint64_t*)v$dataset$train_datatriangle__18_,
//     *(const uint64_t*)v$dataset$train_datatriangle__19_,
//   };
//   const int64_t _test_target[] = {
//     0,0,0,0,0,
//     1,1,1,1,1,
//   };
//   const uint64_t test_len = 10;
//   t$pctr$RawData test_input = f$pctr$newRawData(e$pctr$Size$bit_16, test_len);
//   t$pctr$RawData test_target = f$pctr$newRawData_set(e$pctr$Size$bit_8, test_len, _test_target);
  
//   //? 범위 변환.
//   max_data = 0;
//   min_data = 0ULL-1;
//   for ( uint64_t i=0; i<test_len; i++ )
//   {
//     if ( max_data<_test_input[i] ) max_data = _test_input[i];
//     if ( min_data>_test_input[i] ) min_data = _test_input[i];
//   }
//   for ( uint64_t i=0; i<test_len; i++ )
//     f$pctr$RawData$set(test_input, i, f$pctr$convRange(0, max_data, _test_input[i], 0, (1ULL<<15)-1));


//   for ( uint64_t i=0; i<test_len; i++ )
//   {
//     int64_t out = f$pctr$Model$calc(model, f$pctr$RawData$get(test_input, i));
//     printf("\n target : %s | model_out : %s", (int)f$pctr$RawData$get(test_target, i)?"삼각형":"사각형", (int)out?"삼각형":"사각형");
//     if ( i==4 ) printf("\n");
//   }

  
//   getchar();
//   return( 0 );
// }



#include "./Perceptron-1.h"
#include "./dataset/img_train_set.c"
// #include "./dataset/house_train_set.c"
#include <time.h>


// -- 8*8 도형 이미지 분류 테스트.
//? 레이블 당 8바이트.
//? 사각형 : 0, 삼각형 : 1
int main()
{
  // -- 학습 데이터 준비.
  printf("\n 학습 데이터 준비중... ");
  const uint64_t v$dataset$train_input[] = {
    
    // -- 사각형 데이터.
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


    // -- 삼각형 데이터.
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
  uint64_t train_len = 20;
  t$pctr$RawData input_data = f$pctr$newRawData(e$pctr$Size$bit_16, train_len);
  uint64_t max_data = 0;
  uint64_t min_data = 0ULL-1;


  //? 최소 최대값 구하기.
  for ( uint64_t i=0; i<train_len; i++ )
  {
    if ( max_data<v$dataset$train_input[i] ) max_data = v$dataset$train_input[i];
    if ( min_data>v$dataset$train_input[i] ) min_data = v$dataset$train_input[i];
  }
  // printf("\n 최소 : %llu, 최대 : %llu, 차 : %llu", min_data, max_data, max_data-min_data);


  //? 입력 범위 변환.
  for ( uint64_t i=0; i<train_len; i++ )
    f$pctr$RawData$set(input_data, i, f$pctr$convRange(0, max_data, v$dataset$train_input[i], 0, (1ULL<<15)-1));


  //? 변환된 값의 최소 최대값 구하기.
  max_data = 0;
  min_data = 0ull-1;
  for ( uint64_t i=0; i<train_len; i++ )
  {
    if ( max_data<(uint64_t)f$pctr$RawData$get(input_data, i) ) max_data = (uint64_t)f$pctr$RawData$get(input_data, i);
    if ( min_data>(uint64_t)f$pctr$RawData$get(input_data, i) ) min_data = (uint64_t)f$pctr$RawData$get(input_data, i);
  }
  // printf("\n 최소 : %llu, 최대 : %llu", min_data, max_data);


  //? 두 입력 데이터의 차 중 제일 작은 차 구하기.
  uint64_t min_abs = 0ull-1;
  for ( uint64_t i=0; i<train_len-1; i++ )
  {
    uint64_t selected = v$dataset$train_input[i];
    for ( uint64_t j=i+1; j<train_len; j++ )
    {
      uint64_t calc = selected>v$dataset$train_input[j]?selected-v$dataset$train_input[j]:v$dataset$train_input[j]-selected;
      // printf("\n 차 값 : %llu", calc);
      if ( min_abs>calc ) min_abs = calc;
      if ( calc==0 ) printf("\n 중복 값 인덱스 : %llu, %llu", i, j);
    }
  }
  // printf("\n 최소 차 : %llu", min_abs);


  //? 정답 데이터 준비.
  const uint8_t v$dataset$train_target[] = {
    0,0,0,0,0,0,0,0,0,0,
		1,1,1,1,1,1,1,1,1,1,
	};
  t$pctr$RawData target_data = f$pctr$newRawData(e$pctr$Size$bit_8, train_len);
  for ( uint64_t i=0; i<train_len; i++ )
    f$pctr$RawData$set(target_data, i, v$dataset$train_target[i]);
  printf(" OK");
  getchar();


  //? 입력, 정답 데이터 확인.
  for ( uint64_t i=0; i<train_len; i++ )
  {
    printf("\n input : %llu, target : %llu", f$pctr$RawData$get(input_data, i), f$pctr$RawData$get(target_data, i));
    // printf("\n input : %llu, target : %d", v$dataset$train_input[i], (int)v$dataset$train_target[i]);
  }
  

  // -- 모델 생성.
  printf("\n 모델 생성중... ");
  t$pctr$Model model = f$pctr$_newModel(
    1, (uint64_t[]){ (1ull<<15)+1 },
    100,1,
    3,
    0
  );
  printf(" OK");
  getchar();


  // -- 모델 학습.
  //? 추가로 시간 측정.
  printf("\n 학습 시작... ");
  double start_time = clock()/(double)CLOCKS_PER_SEC;
  f$pctr$Model$_fit(model, input_data, target_data, 1, 0);
  double end_time = clock()/(double)CLOCKS_PER_SEC;
  printf(" OK");


  // -- 걸린 시간 측정.
  printf("\n 학습에 걸린 시간 : %llfs", end_time-start_time);
  getchar();


  // -- 테스트 데이터 확인.
  printf("\n 테스트 데이터로 검증");
  const uint64_t _test_input[] = {
    *(const uint64_t*)v$dataset$train_datasquare__11_,
    *(const uint64_t*)v$dataset$train_datasquare__12_,
    *(const uint64_t*)v$dataset$train_datasquare__14_,
    *(const uint64_t*)v$dataset$train_datasquare__15_,
    *(const uint64_t*)v$dataset$train_datasquare__16_,


    *(const uint64_t*)v$dataset$train_datatriangle__11_,
    *(const uint64_t*)v$dataset$train_datatriangle__16_,
    *(const uint64_t*)v$dataset$train_datatriangle__17_,
    *(const uint64_t*)v$dataset$train_datatriangle__18_,
    *(const uint64_t*)v$dataset$train_datatriangle__19_,
  };
  const int64_t _test_target[] = {
    0,0,0,0,0,
    1,1,1,1,1,
  };
  const uint64_t test_len = 10;
  t$pctr$RawData test_input = f$pctr$newRawData(e$pctr$Size$bit_16, test_len);
  t$pctr$RawData test_target = f$pctr$newRawData_set(e$pctr$Size$bit_8, test_len, _test_target);
  
  //? 범위 변환.
  max_data = 0;
  min_data = 0ULL-1;
  for ( uint64_t i=0; i<test_len; i++ )
  {
    if ( max_data<_test_input[i] ) max_data = _test_input[i];
    if ( min_data>_test_input[i] ) min_data = _test_input[i];
  }
  for ( uint64_t i=0; i<test_len; i++ )
    f$pctr$RawData$set(test_input, i, f$pctr$convRange(0, max_data, _test_input[i], 0, (1ULL<<15)-1));


  for ( uint64_t i=0; i<test_len; i++ )
  {
    int64_t out = f$pctr$Model$calc(model, f$pctr$RawData$get(test_input, i));
    printf("\n target : %s | model_out : %s", (int)f$pctr$RawData$get(test_target, i)?"삼각형":"사각형", (int)out?"삼각형":"사각형");
    if ( i==4 ) printf("\n");
  }

  
  getchar();
  return( 0 );
}