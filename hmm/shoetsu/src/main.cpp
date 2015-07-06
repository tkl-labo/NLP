

#include "util.h"
#include "hmm.h"

#include <iostream>
#include <memory>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

using namespace std;


//=========================
//   Hmm Tests
//=========================

void hmm_learn(int argc, char ** argv)
{
  unique_ptr<Hmm> hmm = make_unique<Hmm>();
  double t0,t1;
  t0 = cur_time();
  hmm->Learn();
  double accuracy = hmm->Test();
  t1 = cur_time();
  printf( "Elapsed Time : %.5f sec\n",t1-t0);
  cout << "Accuracy : " << accuracy << " % " <<endl;
}


//=========================
//   About Options
//=========================

struct opts {
  const char * method;
  opts() {
    method = "learn";
  }
};


void usage(char * prog) {
  opts o;
  fprintf(stderr, "usage:\n");
  fprintf(stderr, "  %s [options]\n", prog);
  fprintf(stderr, "options:\n");
  fprintf(stderr, "  -m,--method learn/ (%s)\n", o.method);
}

opts * parse_cmdline(int argc, char * const * argv, opts * o) {
  static struct option long_options[] = {
    {"method",     required_argument, 0, 'm' },
    {0,         0,                 0,  0 }
  };

  while (1) {
    int option_index = 0;
    int c = getopt_long(argc, argv, "m:",
			long_options, &option_index);
    if (c == -1) break;

    switch (c) {
    case 'm':
      o->method = strdup(optarg);
      break;
    default:
      usage(argv[0]);
      exit(1);
    }
  }
  return o;
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
    hmm_learn(argc,argv);
  }
  return 0;
}



