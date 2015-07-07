#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
using namespace std;

typedef struct node{
  int count;
  double prob;
  
  bool operator<( const struct node& right ) const {
    return prob < right.prob;
  }
} node_t;

typedef struct backpointer{
  double prob;
  vector<string> tag_seq;
} bp_t;

const int K = 1;
map<string, map<string, node_t> > tag_matrix;
map<string, map<string, node_t> > tag_word_matrix;
vector<string> tag_sequence;
map<string, int> words;
vector<int> error(K);
map<string, int> total;
float smt_count = 0.7;
vector<bp_t> k_best(K);

void count(string prev_tag, string current_tag, string word){
  //count tag
  if(tag_matrix[prev_tag].find(current_tag) == tag_matrix[prev_tag].end()){
    tag_matrix[prev_tag][current_tag].count = 1;
    tag_matrix[prev_tag][current_tag].prob = 0;
  }
  else{
    tag_matrix[prev_tag][current_tag].count++;
  }
  //count word
  if(tag_word_matrix[current_tag].find(word) == tag_word_matrix[current_tag].end()){
    tag_word_matrix[current_tag][word].count = 1;
    tag_word_matrix[current_tag][word].prob = 0;
  }
  else{
    tag_word_matrix[prev_tag][word].count++;
  }
  //update total
  if(total.find(current_tag) == total.end()) total[current_tag] = 1;
  else total[current_tag]++;

  words[word];
}

void make_HMM(){
  for(map<string, map<string, node_t> >::iterator prev_tag = tag_matrix.begin(); prev_tag != tag_matrix.end(); prev_tag++){
    for(map<string, node_t>::iterator current_tag = (prev_tag->second).begin(); current_tag != (prev_tag->second).end(); current_tag++){
      //cout << (current_tag->second).count << " tag " << prev_tag->first << " " << total[prev_tag->first] << endl;
      (current_tag->second).prob = (float)(current_tag->second).count / (total[prev_tag->first]);
    }
  }
  for(map<string, map<string, node_t> >::iterator tag = tag_word_matrix.begin(); tag != tag_word_matrix.end(); tag++){
    for(map<string, node_t>::iterator current_word = (tag->second).begin(); current_word != (tag->second).end(); current_word++){
      //cout << (current_word->second).count << " word " << tag->first << " " << total[tag->first] << endl;
      (current_word->second).prob = (float)(current_word->second).count + smt_count / (total[tag->first] + words.size() * smt_count);
    }
    (tag->second)["<UNK>"].prob = smt_count / (total[tag->first] + words.size() * smt_count);
  }
}

void search(vector<string> &input, string tag, int length, double prob){
  for(map<string, node_t>::iterator current_tag = tag_matrix[tag].begin(); current_tag != tag_matrix[tag].end(); current_tag++){
    string word = input[tag_sequence.size()-1];
    double updated_prob;
    if(tag_word_matrix[current_tag->first].find(word) != tag_word_matrix[current_tag->first].end()){
      updated_prob = prob * tag_matrix[tag][current_tag->first].prob * tag_word_matrix[current_tag->first][word].prob;
    }
    else{
      updated_prob = prob * tag_matrix[tag][current_tag->first].prob * tag_word_matrix[current_tag->first]["<UNK>"].prob;
    }
    tag_sequence.push_back(current_tag->first);
    if(tag_sequence.size() == length){
      updated_prob *= tag_matrix[current_tag->first]["END"].prob;
      /*
         cout << "prob" << updated_prob << endl;
         for(int i= 0; i<tag_sequence.size(); i++){
         cout << tag_sequence[i] << " ";
         }
         cout << endl;
         */
      //k_bestを求める
      //sort(k_best.begin(), k_best.end());
      if(k_best[0].prob < updated_prob){
        k_best[0].prob = updated_prob;
        k_best[0].tag_seq = tag_sequence; 
      }
      cout << k_best[0].prob << endl;
      tag_sequence.pop_back();
    }
    else{
      search(input, current_tag->first, length, updated_prob);
      tag_sequence.pop_back();
    }
  }
}

void viterbi(vector<string> &input, vector<string> answer){
  int length = input.size() + 1; //<s>のぶん足す
  tag_sequence.push_back("START");
  search(input, "START", length, 1);

  for(int i = 0; i < length; i++){
    cout << input[i] << " " << answer[i] << " ";
    for(int j = 0; j < K; j++){
      if(k_best[j].tag_seq[i+1] == answer[i]){
          cout << k_best[j].tag_seq[i+1] << " ";
      }
      else{
        cout << "*" << k_best[j].tag_seq[i+1] << " ";
        error[j]++;
      }
    }
    cout << endl;
  }
  cout << endl;
}

int main(int argc, char* argv[]){
  ifstream ifs(argv[1]);
  ifstream ifs_t(argv[2]);
  string input, test_input;
  string prev_tag="START", current_tag, word;
  if(ifs.fail() || ifs_t.fail()){
    cout << "error" << endl;
    return -1;
  }
  while(getline(ifs, input)){
    if (input.length() == 0){
      current_tag = "END";
      word = "</s>";
    }
    else{
      int index = input.find(" ", 0);
      int index2 = input.find(" ", index+1);
      current_tag = input.substr(index+1, index2-index-1);
      word = input.substr(0, index);
    }
    count(prev_tag, current_tag, word);
    if(input.length() == 0) prev_tag = "START";
    else prev_tag = current_tag;
  }
  total["START"] = total["END"];
  make_HMM();
  /*
    for(map<string, node_t>::iterator current_tag = tag_matrix["CD"].begin(); current_tag != tag_matrix["CD"].end(); current_tag++){
      cout << current_tag->first << " " << (current_tag->second).prob << endl;
    }
  */
  int total;
  vector<string> sentence, answer;
  while(getline(ifs_t, test_input)){
    if (test_input.length() == 0){
      viterbi(sentence, answer);
      /*
      for(int i=0; i < sentence.size(); i++){
        cout << sentence[i] << " ";
      }
      cout << endl;
      */
      sentence.clear();
    }
    else{
      int index = test_input.find(" ", 0);
      int index2 = test_input.find(" ", index+1);
      answer.push_back(test_input.substr(index+1, index2-index-1));
      sentence.push_back(test_input.substr(0, index));
      total++;
    }
  }
  cout << "error rate" << endl;
  for(int i = 0; i < K; i++){
    cout << "1. " << (float)(total-error[i])/total << endl;
  }
}

