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
  
  t$pctr$RawData check = f$pctr$newRawData(e$pctr$Size$bit_32, v$house$len);

  
  // -- 모델 준비.
  printf("\n 모델 준비 중. ");
  t$pctr$Model model = f$pctr$_newModel(
    1, (uint64_t[]){ 401749+1 },
    0,0,
    100,
    0
  );
  printf("OK");


  // -- 모델 학습.
  printf("\n 모델 학습 시작. ");
  double start = clock()/(double)CLOCKS_PER_SEC;
  f$pctr$Model$_fit(
    model,
    input_datas, target_datas,
    1, 0
  );
  printf("OK");
  double end = clock()/(double)CLOCKS_PER_SEC;
  printf("\n 학습에 걸린 시간 : %llfms", end-start);


  // -- 메모리 해제.
  f$pctr$Model$release(model);
  f$pctr$RawData$release(input_datas);
  f$pctr$RawData$release(target_datas);
  
  
  getchar();
  return( 0 );
}
