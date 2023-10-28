#include <stdio.h>
#include "./Perceptron-1.h"
#include <time.h>


int main()
{
  clock_t start_time = clock();
  t$pctr$Model model = f$pctr$_newModel(
    2, (uint64_t[]){4,3},
    0,0,
    3,
    0
  );

  clock_t end_time = clock();
  printf("\n 걸린 시간 : %ldms (%.3fs)", end_time-start_time, (end_time-start_time)/1000.0);

  model = f$pctr$Model$release(model);
  
  getchar();
  return( 0 );
}