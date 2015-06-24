



#include "util.h"
#include "n_gram.h"
#include <iostream>
#include <cassert>
#include <memory>
using namespace std;



void NgramTest_LearnAndSave(int argc, char ** argv)
{
  const int N = stoi(argv[2]);
  string filename = (N == 2) ? "bi_gram.dat" : "tri_gram.dat";
  assert(N < 4);

  //unique_ptr<NGram> n_gram = make_unique<LaplaceSmoothedNGram>(N);
  unique_ptr<NGram> n_gram = make_unique<NGram>(N);


  double t0,t1;
  t0 = cur_time();
  n_gram->Learn();
  cout << "Output: " << n_gram->CreateRandomSentence() << endl;
  n_gram->Save(filename);
  t1 = cur_time();
  printf( "Elapsed Time: %.5f sec\n",t1-t0);
}

void NgramTest_Load(int argc, char ** argv)
{
  const int N = stoi(argv[2]);
  assert(N < 4);
  string filename = (N == 2) ? "bi_gram.dat" : "tri_gram.dat";

  //unique_ptr<NGram> n_gram = make_unique<LaplaceSmoothedNGram>(N);
  unique_ptr<NGram> n_gram = make_unique<NGram>(N);
  double t0,t1;
  t0 = cur_time();
  //n_gram->Load("n_gram_2012-01-01.dat");
  n_gram->Load(filename);
  cout << "Finish Loading..." << endl;
  cout << "Output: " << n_gram->CreateRandomSentence() << endl;
  t1 = cur_time();
  printf( "Elapsed Time: %.5f sec\n",t1-t0);
}



int main(int argc, char** argv){
  
  if (argc < 3){
    cout << "./a.out (learn|load) N "<< endl;
    exit(1);
  }
  srand((unsigned int)(cur_time()));
  string method = argv[1];

  if (method == "learn"){
    NgramTest_LearnAndSave(argc, argv);
  }else if (method == "load"){
    NgramTest_Load(argc, argv);
  }else{
    cout << "./a.out (learn|load) N "<< endl;
    exit(1);
  }
  return 0;
}


