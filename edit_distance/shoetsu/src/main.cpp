




#include "edit_distance.h"
#include "util.h"

#include <iostream>
#include <string>

using namespace std;



void EditDistanceTest(int argc, char** argv){

  
  const string a = argc > 1 ? argv[1] : "teststring1"; 
  const string b = argc > 2 ? argv[2] : "teststring2";

  double t0,t1;
  t0 = cur_time();
  calcEditDistance(a,b);
  t1 = cur_time();
  printf( "%.5f sec.\n",t1-t0);

}


int main(int argc, char** argv){
  
  EditDistanceTest(argc, argv);
  
  return 0;
}


