#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "./dataset/house_train_set.c"
#include "./Perceptron-1.h"

int main()
{
  // -- 학습 데이터 준비.
  printf("\n 데이터 준비 중. ");
  t$pctr$RawData input_datas = f$pctr$newRawData_set(e$pctr$Size$bit_32, v$house$len, v$house$input_data);
  t$pctr$RawData target_datas = f$pctr$newRawData_set(e$pctr$Size$bit_8, v$house$len, v$house$target_data);
  printf("OK");
  
  
  // -- 모델 준비.
  printf("\n 모델 준비 중. ");
  t$pctr$Model model = f$pctr$_newModel(
    1, (uint64_t[]){ (1ULL<<29)+1 },
    0,0,
    25,
    0
  );
  printf("OK");


  // -- 모델 학습.
  printf("\n 모델 학습 시작. ");
  clock_t start = clock();
  f$pctr$Model$_fit(
    model,
    input_datas, target_datas,
    1, 0
  );
  printf("OK");
  clock_t end = clock();

  printf("\n 학습에 걸린 시간 : %ldms", start-end);


  // -- 메모리 해제.
  f$pctr$Model$release(model);
  f$pctr$RawData$release(input_datas);
  f$pctr$RawData$release(target_datas);
  
  
  getchar();
  return( 0 );
}
