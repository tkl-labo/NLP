



#include "util.h"
#include "n_gram.h"
#include <iostream>

#include <memory>
using namespace std;



void NgramTest_LearnAndSave(int argc, char ** argv)
{
  unique_ptr<NGram> n_gram = make_unique<NGram>();
  double t0,t1;
  t0 = cur_time();
  n_gram->Learn();
  cout << "Output: " << n_gram->CreateRandomSentence() << endl;
  n_gram->Save("n_gram.dat");
  t1 = cur_time();
  printf( "Elapsed Time: %.5f sec\n",t1-t0);
}

void NgramTest_Load(int argc, char ** argv)
{
  unique_ptr<NGram> n_gram = make_unique<NGram>();
  double t0,t1;
  t0 = cur_time();
  n_gram->Load("n_gram.dat");
  cout << "Output: " << n_gram->CreateRandomSentence() << endl;
  t1 = cur_time();
  printf( "Elapsed Time: %.5f sec\n",t1-t0);
}


int main(int argc, char** argv){
  //setlocale( LC_ALL, "ja_JP.UTF-8" );
  srand((unsigned int)(cur_time()));
  //NgramTest_LearnAndSave(argc, argv);
  NgramTest_Load(argc, argv);
  return 0;
}


