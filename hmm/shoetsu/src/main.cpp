

#include "util.h"
#include "hmm.h"

#include <iostream>
#include <memory>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

using namespace std;



//=========================
//   About Options
//=========================

struct opts {
  const char * method;
  int rareword_threshold;
  opts() {
    method = "learn";
    rareword_threshold = 10;
  }
};


void usage(char * prog) {
  opts o;
  fprintf(stderr, "usage:\n");
  fprintf(stderr, "  %s [options]\n", prog);
  fprintf(stderr, "options:\n");
  fprintf(stderr, "  -m,--method  (%s)\n", o.method);
  fprintf(stderr, "  -t,--threshold  (%d)\n", o.rareword_threshold);
}

opts * parse_cmdline(int argc, char * const * argv, opts * o) {
  static struct option long_options[] = {
    {"method",     required_argument, 0, 'm' },
    {"rareword_threshold",     required_argument, 0, 't' },
    {0,         0,                 0,  0 }
  };

  while (1) {
    int option_index = 0;
    int c = getopt_long(argc, argv, "m:t:",
			long_options, &option_index);
    if (c == -1) break;

    switch (c) {
    case 'm':
      o->method = strdup(optarg);
      break;
    case 't':
      o->rareword_threshold = atoi(strdup(optarg));
      break;
    default:
      usage(argv[0]);
      exit(1);
    }
  }
  return o;
}


//=========================
//   Hmm Tests
//=========================

void hmm_learn(const opts o)
{
  const string TRAIN_DATA = "data/train.txt";
  const string TEST_DATA = "data/test_train_mini.txt";
  unique_ptr<Hmm> hmm = make_unique<Hmm>(o.rareword_threshold);
  double t0,t1,t2;
  t0 = cur_time();
  hmm->Learn(TRAIN_DATA);
  t1 = cur_time();
  hmm->Test(TEST_DATA);
  t2 = cur_time();
 
  cerr << "Train Data        : " << TRAIN_DATA << endl;
  cerr << "Test  Data        : " << TEST_DATA  << endl;
  cerr << "Elapsed Time (Learn) : " << (t1-t0) << " sec\n";
  cerr << "Elapsed Time (Test)  : " << (t2-t1) << " sec\n";
}

//=========================
//         Main
//=========================


int main(int argc, char** argv){
  
  if (argc < 1){
    exit(1);
  }
  srand((unsigned int)(cur_time()));

  opts o;
  parse_cmdline(argc, argv, &o);
  const string method = o.method;

  if(method == "learn"){
    hmm_learn(o);
  }
  return 0;
}



