



#include "util.h"
#include "n_gram.h"
#include <iostream>

#include <string>
#include <wchar.h>
#include <memory>
using namespace std;



void NgramTest(int argc, char ** argv)
{

  unique_ptr<NGram> n_gram = make_unique<NGram>();
  double t0,t1;
  t0 = cur_time();
  n_gram->Learn();
  n_gram->CreateRandomSentence();
  t1 = cur_time();
  printf( "%.5f sec.\n",t1-t0);
}


int main(int argc, char** argv){
  setlocale( LC_ALL, "ja_JP.UTF-8" );

  NgramTest(argc, argv);
  return 0;
}


