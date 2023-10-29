#include <stdio.h>
#include <stdint.h>
#include "./Perceptron-1.h"
#include <time.h>


int main()
{
  clock_t start_time = 0;
  clock_t end_time = 0;
  int64_t result = 0;
  t$pctr$Model model = f$pctr$structModel();
  
  start_time = clock();
  model = f$pctr$_newModel(
    2, (uint64_t[]){40,30},
    12,2,
    10,
    1
  );
  result = f$pctr$Model$calc(model, 120);
  end_time = clock();

  printf("\n모델 연산 결과 : %lld", result);
  printf("\n걸린 시간 : %ldms (%.3fs)", end_time-start_time, (end_time-start_time)/1000.0);

  
  f$pctr$Model$release(model);

  getchar();
  return( 0 );
}
