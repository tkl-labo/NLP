
#include "util.h"

#include <sys/time.h>
using namespace std;

double cur_time(){
  struct timeval tp[1];
  gettimeofday(tp,0);
  return tp->tv_sec + 1.0e-6 * tp->tv_usec;
}




