#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <algorithm>
#include <chrono>

using namespace std;

const int K = 1;
map<string, map<string, int> > tag_matrix;
map<string, map<string, int> > tag_word_matrix;

map<string, int> total;
map<string, int> tag_id;
map<string, int> word_id;
vector<int> error(K);
float smt_count = 0.3;
vector<int> k_best(K);

string get_string(int id, map<string, int> &matrix){
  for(map<string, int>::iterator i=matrix.begin(); i != matrix.end(); i++){
    if(i->second == id) return i->first;
  }
  return 0;
}

void count(string prev_tag, string current_tag, string word){
  //count tag
  if(tag_matrix[prev_tag].find(current_tag) == tag_matrix[prev_tag].end()){
    tag_matrix[prev_tag][current_tag]= 1;
  }
  else{
    tag_matrix[prev_tag][current_tag]++;
  }
  //count word
  if(tag_word_matrix[current_tag].find(word) == tag_word_matrix[current_tag].end()){
    tag_word_matrix[current_tag][word]= 1;
  }
  else{
    tag_word_matrix[current_tag][word]++;
  }
  //update total
  if(total.find(current_tag) == total.end()) total[current_tag] = 1;
  else total[current_tag]++;

  word_id[word];
}

void set_id(){
  tag_id["END"]=0;
  for(map<string, map<string, int> >::iterator prev_tag = tag_matrix.begin(); prev_tag != tag_matrix.end(); prev_tag++){
    tag_id[prev_tag->first] = tag_id.size()-1;
  }
  word_id["<UNK>"] = word_id.size()-1;
  int index = 0;
  for(map<string, int>::iterator word = word_id.begin(); word != word_id.end(); word++){
    word_id[word->first] = index;
    index++;
  }
}

void make_HMM_graph(vector<vector<int> > &transition_count, vector<vector<int> > &emission_count, vector<vector<double> > &transition_prob, vector<vector<double> > &emission_prob){
  int size = tag_id.size();
  for(int i=0; i<size; i++){
    transition_count[i] = *(new vector<int>(size));
    transition_prob[i] = *(new vector<double>(size));
  }
  for(int i=0; i<word_id.size(); i++){
    emission_count[i] = *(new vector<int>(size));
    emission_prob[i] = *(new vector<double>(size));
  }
  for(map<string, map<string, int> >::iterator prev_tag = tag_matrix.begin(); prev_tag != tag_matrix.end(); prev_tag++){
    int prev_id = tag_id[prev_tag->first];
    //cout << prev_tag->first << ":" << endl;
    for(map<string, int>::iterator current_tag = (prev_tag->second).begin(); current_tag != (prev_tag->second).end(); current_tag++){
      //cout<< current_tag->first << endl;
      int current_id = tag_id[current_tag->first];
      transition_count[prev_id][current_id] = current_tag->second;
      transition_prob[prev_id][current_id] = (double)current_tag->second / (total[prev_tag->first]);
    }
  }
  for(map<string, map<string, int> >::iterator tag = tag_word_matrix.begin(); tag != tag_word_matrix.end(); tag++){
    int current_tag_id = tag_id[tag->first];
    for(map<string, int>::iterator word = (tag->second).begin(); word != (tag->second).end(); word++){
      int current_word_id = word_id[word->first];
      emission_count[current_word_id][current_tag_id] = word->second;
      emission_prob[current_word_id][current_tag_id] = (float)(word->second + smt_count) / (total[tag->first] + word_id.size() * smt_count);
    }
    cout << endl;
    //cout << tag->first << total[tag->first] << " " << word_id.size() << smt_count / (total[tag->first] + word_id.size() * smt_count) << endl;
   emission_prob[word_id["<UNK>"]][current_tag_id] = smt_count / (total[tag->first] + word_id.size() * smt_count);
  }
}

void viterbi(vector<string> &input, vector<string> &answer, vector<vector<double> > &transition_prob, vector<vector<double> > &emission_prob){
  vector<vector<double> > prob_table(input.size());
  vector<vector<int> > backpointer(input.size());
  for(int i=0; i<input.size(); i++){
    prob_table[i] = *(new vector<double>(tag_id.size()));
    backpointer[i] = *(new vector<int>(word_id.size()));
  }
  for(int i=0; i<tag_id.size(); i++){
    prob_table[0][i] = transition_prob[tag_id["START"]][i] * emission_prob[word_id[input[0]]][i];
    cout << "d" << get_string(i, tag_id) << " "  << prob_table[0][i] << endl;
    backpointer[0][i] = 0;
  }
  double max = 0, tmp_prob, e_prob;
  int max_index;
  for(int i=1; i<input.size(); i++){
    cout << "aaa" << endl;
    cout << get_string(i, tag_id) << ":" << endl;
    for(int j=0; j<tag_id.size(); j++){
      cout << " " << get_string(j, tag_id)<<endl;
      for(int k=0; k<tag_id.size(); k++){
        e_prob = (emission_prob[i][j] != 0 ? emission_prob[i][j] : emission_prob[word_id["<UNK>"]][j]);
        if((tmp_prob = prob_table[i-1][k] * transition_prob[k][j] * e_prob) >= max) {
          //cout << prob_table[i-1][k] << " " << e_prob << " " << " " << transition_prob[k][j] << " " << tmp_prob << endl;
          max = tmp_prob;
          max_index = k;
        }
      }
      prob_table[i][j] = max;
      backpointer[i][j] = max_index;
      cout << "  " << get_string(max_index, tag_id) << max << " " << get_string(max_index, tag_id) << endl;
      max = 0;
    }
  }
  vector<int> k_best(K, -1);
  int last_index = input.size()-1;
  for(int i=0; i<tag_id.size(); i++){
    sort(k_best.begin(), k_best.end());
    tmp_prob = prob_table[last_index][i] * transition_prob[i][tag_id["END"]];
    if(k_best[0] < tmp_prob) k_best[0] = i;
  }
  //print result
  vector<vector<int> > backtrace(K);
  int next_tag;
  for(int i=0; i<K; i++){
    backtrace[i].push_back(k_best[i]);
    next_tag = backpointer[last_index][k_best[i]];
    for(int index=last_index-1; index >=0; index--){
      backtrace[i].push_back(next_tag);
      next_tag = backpointer[index][next_tag];
    }
  }
  cout << get_string(k_best[0],tag_id) << "best";
  for(int i = 0; i < input.size(); i++){
    //cout << input[i] << " " << answer[i] << " ";
    for(int j = 0; j < K; j++){
      if(backtrace[j].back() == tag_id[answer[i]]){
        //cout << get_string(backtrace[j].back(), tag_id) << " ";
      }
      else{
        //cout << "*" << get_string(backtrace[j].back(), tag_id) << " ";
        error[j]++;
      }
      backtrace[j].pop_back();
    }
    //cout << endl;
  }
  //cout << endl;
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
  set_id();

  int t_size = tag_id.size();
  int w_size = word_id.size();
  vector<vector<int> > transition_count(t_size);
  vector<vector<int> > emission_count(w_size);
  vector<vector<double> > transition_prob(t_size);
  vector<vector<double> > emission_prob(w_size);
  make_HMM_graph(transition_count, emission_count, transition_prob, emission_prob);
  //debug
  /*
  cout << "Total" << total["START"] << total["END"];
  for(int j=0; j<tag_id.size(); j++){
    cout << get_string(j, tag_id) << endl;
  for(int i=0; i<tag_id.size();i++){
    cout << get_string(i, tag_id) << " " << transition_count[j][i] << " " << transition_prob[j][i] << endl;
  }
  cout << endl;
  }
  cout << tag_matrix["START"]["NNP"] << endl;
  cout << transition_count[tag_id["START"]][tag_id["NNP"]] << endl;
  cout << "word" <<  emission_count[word_id["<UNK>"]][tag_id["IN"]] << " " << emission_prob[word_id["<UNK>"]][tag_id["IN"]] << endl; 
  */
  int total;
  vector<string> sentence, answer;
  while(getline(ifs_t, test_input)){
    if (test_input.length() == 0){
      cout << input[0] << endl;
      viterbi(sentence, answer, transition_prob, emission_prob);
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
    cout << i+1 << ". " << (float)(total-error[i])/total << endl;
  }
}

