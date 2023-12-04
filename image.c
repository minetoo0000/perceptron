#include "./Perceptron-1.h"
#include "./dataset/square-dataset.c"
#include "./dataset/triangle-dataset.c"
#include <time.h>

/**
 * 삼각형과 사각형을 분류하는 모델 구현.
 * 
 * target
 * 사각형 : 0, 삼각형 : 1
*/

int main()
{
  uint64_t max_data = 0;
  uint64_t min_data = 0ull-1;
  
  // -- 학습 데이터 준비.
  printf("\n 학습 데이터 준비중...");
  //? square 24 + triangle 24
  uint64_t train_square_len = 16;
  uint64_t train_triangle_len = 16;
  uint64_t train_len = train_square_len+train_triangle_len;

  // -- 학습 이미지 데이터 가져오기.
  uint64_t train_dataset_origin[train_len];
  {
    for ( uint64_t i=0; i<train_square_len; i++ )
      train_dataset_origin[i] = *v$dataset$square$allArray[i];
    for ( uint64_t i=train_square_len; i<train_len; i++ )
      train_dataset_origin[i] = *v$dataset$triangle$allArray[i-train_square_len];
  }
  
  // -- 이미지 데이터 전처리 : 크기 줄이기.
  t$pctr$RawData train_dataset = f$pctr$newRawData(e$pctr$Size$bit_32, train_len);
  {
    //? 최대 최소 구하기.
    max_data = 0;
    min_data = 0ull-1;
    for ( uint64_t i=0; i<train_len; i++ )
    {
      if ( max_data<train_dataset_origin[i] ) max_data = train_dataset_origin[i];
      if ( min_data>train_dataset_origin[i] ) min_data = train_dataset_origin[i];
    }

    //? 범위 변환.
    for ( uint64_t i=0; i<train_len; i++ )
    {
      f$pctr$RawData$set(
        train_dataset,
        i,
        f$pctr$convRange(min_data, max_data, train_dataset_origin[i], 0, (1ull<<16)-1)
      );
    }
  }
  
  // {
  //   uint64_t max_data = 0;
  //   uint64_t min_data = 0ull-1;
  //   for ( uint64_t i=0; i<train_len; i++ )
  //   {
  //     if ( max_data<f$pctr$RawData$get(train_dataset, i) ) max_data = f$pctr$RawData$get(train_dataset, i);
  //     if ( min_data>f$pctr$RawData$get(train_dataset, i) ) min_data = f$pctr$RawData$get(train_dataset, i);
  //   }
  //   printf("\n 최소 : %llu, 최대 : %llu", min_data, max_data);
  // }
  
  // -- 정답 데이터 준비.
  uint8_t target_dataset_origin[train_len];
  t$pctr$RawData target_dataset = f$pctr$newRawData(e$pctr$Size$bit_8, train_len);
  {
    for ( uint64_t i=0; i<train_square_len; i++ )
      target_dataset_origin[i] = 0;
    for ( uint64_t i=train_square_len; i<train_len; i++ )
      target_dataset_origin[i] = 1;
    for ( uint64_t i=0; i<train_len; i++ )
      f$pctr$RawData$set(target_dataset, i, target_dataset_origin[i]);
  }
  printf(" OK");
  getchar();

  // -- 모델 준비.
  printf("\n 모델 준비...");
  t$pctr$Model model = f$pctr$_newModel(
    1, (uint64_t[]){ (1ull<<16) },
    3,2,
    3,
    0
  );
  printf(" OK");
  getchar();

  //////////////////////////////////////
  {
    for ( uint64_t i=0; i<train_len; i++ )
    {
      printf("\n conv result : %28llu  %18llu", train_dataset_origin[i], f$pctr$RawData$get(train_dataset, i));
      if ( i==train_square_len-1 ) printf("\n");
    }
  }

  // -- 모델 학습.
  printf("\n 모델 학습 시작...");
  double start_time = (double)clock()/CLOCKS_PER_SEC;
  f$pctr$Model$_fit(model, train_dataset, target_dataset, 1, 0);
  double end_time = (double)clock()/CLOCKS_PER_SEC;
  printf("\n 학습에 걸린 시간 : %llfs", end_time-start_time);
  printf(" OK");
  getchar();

  // -- 모델 테스트.
  printf("\n 모델 테스트...");
  uint64_t total_data_len = v$dataset$square$allArray_LEN+v$dataset$triangle$allArray_LEN;
  uint64_t test_len = total_data_len-train_len;
  uint64_t test_dataset_origin[test_len];
  t$pctr$RawData test_dataset = f$pctr$newRawData(e$pctr$Size$bit_32, test_len);
  //? 테스트 데이터 가져오기.
  {
    //? 학습에 사용되지 않은 나머지 데이터로 테스트 데이터 만듬.
    for ( uint64_t i=train_square_len; i<v$dataset$square$allArray_LEN; i++ )
      test_dataset_origin[i-train_square_len] = *v$dataset$square$allArray[i];
    for ( uint64_t i=train_triangle_len; i<v$dataset$triangle$allArray_LEN; i++ )
      test_dataset_origin[i-train_triangle_len+(v$dataset$square$allArray_LEN-train_square_len)] = *v$dataset$triangle$allArray[i];
    for ( uint64_t i=0; i<test_len; i++ )
      f$pctr$RawData$set(
        test_dataset,
        i,
        f$pctr$convRange(min_data, max_data, test_dataset_origin[i], 0, (1ull<<16)-1)
      );
  }
  
  //? 모델 실행.
  {
    uint64_t correct_count = 0;
    for ( uint64_t i=0; i<test_len; i++ )
    {
      int64_t model_out = f$pctr$Model$calc(model, f$pctr$RawData$get(test_dataset, i));
      char correct = 0;
      if ( i<v$dataset$square$allArray_LEN-train_square_len )
      {
        if ( model_out==0 )
        {
          correct = 'O';
          correct_count++;
        }
        else correct = 'X';
      }
      else
      {
        if ( model_out==1 )
        {
          correct = 'O';
          correct_count++;
        }
        else correct = 'X';
      }
      
      if ( i==(v$dataset$square$allArray_LEN-train_square_len) ) printf("\n");

      printf(
        "\n target:%d | model_out:%d | correct:%c",
        //? 전체 테스트 데이터 중 절반 이하는 사각형, 그 초과는 삼각형.
        (int)(i<(v$dataset$square$allArray_LEN-train_square_len)?0:1),
        (int)model_out,
        correct
      );
    }
    printf("\n total_correct:%llu/%llu", correct_count, test_len);
    printf("\n test_accuracy:%.4llf%%", (double)correct_count/test_len*100);
  }
  printf(" OK");
  getchar();

  
  return( 0 );
}