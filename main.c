#include <stdio.h>
#include "./Perceptron.h"


int main()
{
  uint64_t ranges[2] = { 2,2 };

  t$pctr$Model model = f$pctr$newModel(2, ranges, 0, 2, -1);
  printf("predict : %lld\n", f$pctr$Model$predict_raw(model, 99));

  f$pctr$Model$releaseModel(model);
  
  getchar();
  return( 0 );
}