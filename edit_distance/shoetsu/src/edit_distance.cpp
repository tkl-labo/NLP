

#include "edit_distance.h"
#include "util.h"

#include <memory>
#include <iostream>
#include <algorithm>
#include <tuple>

using namespace std;


static void showDPMap(const string &a,const string  &b, const EDCost_t arr[]){
  const size_t al = a.length() + 1;
  const size_t bl = b.length() + 1;
  int i,j;
  cout << "    ";
  cout << "#    ";
  for (j = 0; j < a.length() ; j++){
    printf("%c    ",a[j]);
  }
  cout << endl;
  for (i = 0; i < bl ; i++){
    if (i > 0){
      printf("%c ",b[i-1]);
    }else{
      cout << "# ";
    }
    for (j = 0; j < al ; j++){
      printf("%4.1f ",arr[i * al + j]);
    }
    cout << endl;
  }
}

static void showAlignedStrings(const string &a, const string &b, tuple<int, int> backptr_arr[]){
  const size_t al = a.length() + 1;
  const size_t bl = b.length() + 1;

  
  string a_aligned = "";
  string b_aligned = "";
  tuple <int, int> backptr = backptr_arr[al * bl -1]; 

  int i = bl - 1;
  int j = al - 1;

  while(!(i < 0 && j < 0)){
    backptr = backptr_arr[j + i * al]; 

    if(get<0>(backptr) < i){
      if(get<1>(backptr) < j){
	a_aligned = a[j-1] + a_aligned;
	b_aligned = b[i-1] + b_aligned;
      }else{
	a_aligned = "*"  + a_aligned;
	b_aligned = b[i-1] + b_aligned;
      }
    }else{
	a_aligned = a[j-1] + a_aligned;
	b_aligned = "*"  + b_aligned;
    }
    i = get<0>(backptr);
    j = get<1>(backptr);
  }
  cout << "" <<endl; 

  cout << "a : " << a_aligned << endl;
  cout << "b : " << b_aligned << endl;
}

//for debug
static void showBackptrMap(const string &a, const string &b, tuple<int, int> backptr_arr[]){
  int i,j;
  const int al = a.length()+1;
  const int bl = b.length()+1;

  cout << "       ";
  for (j = 0; j < al ; j++){
    printf("%2d       ",j);
  }
  printf("\n");
  for(i=0;i<bl;i++){
    printf("%2d  ", i);
    for(j=0;j<al;j++){
      printf("(%2d, %2d) ",get<0>(backptr_arr[j + i * al]),get<1>(backptr_arr[j + i * al]));
    }
    printf("\n");
  }
  
}


inline static EDCost_t PermutationCost(const char a, const char b){
  const EDCost_t P_COST = 2;
  if(a == b){
    return 0;
  }

  return P_COST;
}

EDCost_t calcEditDistance(const string &a, const string &b){
  //削除、挿入のコスト
  const EDCost_t D_COST = 1;
  const EDCost_t I_COST = 1;
  
  const size_t al = a.length() + 1;
  const size_t bl = b.length() + 1;
 
  EDCost_t *cost_arr = new EDCost_t[al * bl];
  tuple<int, int> *backptr_arr = new tuple<int, int>[al * bl];

  int i,j;

  const int END = -1; //適当な負の数 

  // initialize
  cost_arr[0] = 0;
  backptr_arr[0] = make_tuple(END, END);
  
  for (j = 1; j < al ; j++){
    cost_arr[j] = j * I_COST;
    backptr_arr[j] = make_tuple(0, j-1);
  }

  for (i = 1; i < bl ; i++){
    cost_arr[i * al] = i * I_COST;
    backptr_arr[i * al] = make_tuple(i-1, 0);
  }

  // exec DP
  for(i = 1; i < bl;i++){
    for(j = 1; j < al;j++){
      
      // cost_arr[j + i * al] = min({cost_arr[j-1 + (i-1) * al] + p_cost, 
      // 			     cost_arr[j-1 + i * al] + I_COST, 
      // 			     cost_arr[j + (i-1) * al] + D_COST}); 

      const float pc = cost_arr[j-1 + (i-1) * al] + PermutationCost(a[j-1], b[i-1]);
      const float ic = cost_arr[j-1 + i * al] + I_COST;
      const float dc = cost_arr[j + (i-1) * al] + D_COST;

      if (pc > ic){
	if (dc > ic){
	  cost_arr[j + i * al] = ic;
	  backptr_arr[j + i * al] = make_tuple(i, j-1);
	}else{
	  cost_arr[j + i * al] = dc;
	  backptr_arr[j + i * al] = make_tuple(i-1, j);
	}
      }else{
	if (pc > dc){
	  cost_arr[j + i * al] = dc;
	  backptr_arr[j + i * al] = make_tuple(i-1, j);
	}else{
	  cost_arr[j + i * al] = pc;
	  backptr_arr[j + i * al] = make_tuple(i-1, j-1);
	}
      }
    }
  }

  // show cost map
  showDPMap(a, b, cost_arr);
  //showBackptrMap(a,b,backptr_arr);
  showAlignedStrings(a, b, backptr_arr);

  const EDCost_t result = cost_arr[al * bl - 1]; 
  cout << "Edit Distance : " << result << endl;
  
  delete backptr_arr; 
  delete cost_arr;
  return result;
}
