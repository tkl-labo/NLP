



#include "util.h"
#include "n_gram.h"
#include <iostream>
#include <cassert>
#include <memory>
using namespace std;



void NgramTest_Learn(int argc, char ** argv)
{
  cout << "<Learn>" <<endl;
  const int N = stoi(argv[2]);
  string filename = (N == 2) ? "data/bi_gram.dat" : "data/tri_gram.dat";
  assert(N < 4);

  unique_ptr<NGram> n_gram = make_unique<NGram>(N);

  double t0,t1;
  t0 = cur_time();
  n_gram->Learn();
  //cout << "Output: " << n_gram->CreateRandomSentence() << endl;
  n_gram->Save(filename);
  cout << "Saved File  : " << filename << endl;
  t1 = cur_time();
  printf( "Elapsed Time: %.5f sec\n",t1-t0);
}

void NgramTest_Create(int argc, char ** argv)
{
  cout << "<Create>" <<endl;
  const int N = stoi(argv[2]);
  assert(N < 4);
  string filename = argv[3];
  //unique_ptr<NGram> n_gram = make_unique<LaplaceSmoothedNGram>(N);
  unique_ptr<NGram> n_gram = make_unique<NGram>(N);
  double t0,t1;
  t0 = cur_time();
  //n_gram->Load("n_gram_2012-01-01.dat");
  n_gram->Load(filename);
  cout << "Loaded N-Gram Data: " << filename  << endl;
  cout << "Output: " << n_gram->CreateRandomSentence() << endl;
  t1 = cur_time();
  printf( "Elapsed Time: %.5f sec\n",t1-t0);
}

void NgramTest_Perplexity(int argc, char ** argv)
{
  cout << "<Perplexity>" <<endl;
  const int N = stoi(argv[2]);
  assert(N < 4);
  string filename = argv[3];
  unique_ptr<LaplaceSmoothedNGram> n_gram = make_unique<LaplaceSmoothedNGram>(N);
  //unique_ptr<NGram> n_gram = make_unique<LaplaceSmoothedNGram>(N);
  //unique_ptr<NGram> n_gram = make_unique<NGram>(N);
  double t0,t1;
  t0 = cur_time();
  n_gram->Load(filename);
  cout << "Loaded N-Gram Data: " << filename  << endl;
  //n_gram->Perplexity(split("あけた な ー う ！ ！"));
  n_gram->PerplexityTest();
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
    NgramTest_Learn(argc, argv);
  }else if (method == "create"){
    NgramTest_Create(argc, argv);
  }else if (method == "perplexity"){
    NgramTest_Perplexity(argc, argv);
  }else{
    cout << "Illegal Argments."<< endl;
    exit(1);
  }
  return 0;
}


