#include "./Perceptron-1.h"
#include <time.h>


int main( int argc, char* argv[] )
{
  clock_t start_time = 0;
  clock_t end_time = 0;
  int64_t result = 0;

  printf("\n시작.");

  // uint64_t input_range[] = { 4, 3 };
  uint64_t input_range[] = { 14 };
  t$pctr$RawData input_data = f$pctr$newRawData(e$pctr$Size$bit_8, 6);
  f$pctr$RawData$set(input_data, 0, 13);
  f$pctr$RawData$set(input_data, 1, 10);
  f$pctr$RawData$set(input_data, 2, 1);
  f$pctr$RawData$set(input_data, 3, 8);
  f$pctr$RawData$set(input_data, 4, 4);
  f$pctr$RawData$set(input_data, 5, 6);
  t$pctr$RawData target_data = f$pctr$newRawData(e$pctr$Size$bit_8, 6);
  f$pctr$RawData$set(target_data, 0, 2);
  f$pctr$RawData$set(target_data, 1, 2);
  f$pctr$RawData$set(target_data, 2, 0);
  f$pctr$RawData$set(target_data, 3, 0);
  f$pctr$RawData$set(target_data, 4, 0);
  f$pctr$RawData$set(target_data, 5, 1);
  t$pctr$Model model = f$pctr$_newModel(
    1, input_range,
    0,0,
    4,
    0
  );
  start_time = clock()/CLOCKS_PER_SEC;
  f$pctr$Model$_fit(model, input_data, target_data, 1, 0);
  end_time = clock()/CLOCKS_PER_SEC;
  printf("\n학습에 걸린 시간 : %ldms (%.3fs)", end_time-start_time, (end_time-start_time)/1000.0);

  for ( int i=0; i<=13; i++ )
  {
    result = f$pctr$Model$calc(model, i);
    printf("\n 모델 연산 | in:%d, out:%lld", i, result);
  }

  model = f$pctr$Model$release(model);

  getchar();
  return( 0 );
}
