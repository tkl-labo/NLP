#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <regex>

using namespace std;

const int K = 1;
unordered_map<string, unordered_map<string, int> > tag_matrix;
unordered_map<string, unordered_map<string, int> > tag_word_matrix;

unordered_map<string, int> total;
unordered_map<string, int> tag_id;
unordered_map<string, int> word_id;
vector<int> error(K);
float smt_count = 0.06;

int max_count_tag() {
  int max = 0;
  for(unordered_map<string, int>::iterator cur=total.begin(); cur != total.end(); cur++){
    if(max < cur->second) max = cur->second;
  }
  return max;
}

string get_string(int id, unordered_map<string, int> &matrix){
  for(unordered_map<string, int>::iterator i=matrix.begin(); i != matrix.end(); i++){
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
  for(unordered_map<string, unordered_map<string, int> >::iterator prev_tag = tag_matrix.begin(); prev_tag != tag_matrix.end(); prev_tag++){
    tag_id[prev_tag->first] = tag_id.size()-1;
  }
  word_id["<UNK>"] = word_id.size()-1;
  int index = 0;
  for(unordered_map<string, int>::iterator word = word_id.begin(); word != word_id.end(); word++){
    word_id[word->first] = index;
    index++;
  }
}

void make_HMM_graph(vector<vector<double> > &transition_prob, vector<vector<double> > &emission_prob){
  int size = tag_id.size();
  for(int i=0; i<size; i++){
    //transition_count[i] = *(new vector<int>(size));
    transition_prob[i] = *(new vector<double>(size));
  }
  for(int i=0; i<word_id.size(); i++){
    //emission_count[i] = *(new vector<int>(size));
    emission_prob[i] = *(new vector<double>(size));
  }
  for(unordered_map<string, unordered_map<string, int> >::iterator prev_tag = tag_matrix.begin(); prev_tag != tag_matrix.end(); prev_tag++){
    int prev_id = tag_id[prev_tag->first];
    //cout << prev_tag->first << ":" << endl;
    for(unordered_map<string, int>::iterator current_tag = (prev_tag->second).begin(); current_tag != (prev_tag->second).end(); current_tag++){
      //cout<< current_tag->first << endl;
      int current_id = tag_id[current_tag->first];
      //transition_count[prev_id][current_id] = current_tag->second;
      transition_prob[prev_id][current_id] = (double)current_tag->second / (total[prev_tag->first]);
    }
  }
  double modify;
  for(unordered_map<string, unordered_map<string, int> >::iterator tag = tag_word_matrix.begin(); tag != tag_word_matrix.end(); tag++){
    int current_tag_id = tag_id[tag->first];
    modify = (float)total[tag->first] / max_count_tag();
    for(unordered_map<string, int>::iterator word = (tag->second).begin(); word != (tag->second).end(); word++){
      int current_word_id = word_id[word->first];
      //emission_count[current_word_id][current_tag_id] = wordsecond;
      emission_prob[current_word_id][current_tag_id] = (float)(word->second + smt_count * modify) / (total[tag->first] + word_id.size() * smt_count * modify);
      if(tag->first == "VBN" && word->first == "spread"){
        cout << tag->first << emission_prob[current_word_id][current_tag_id] << " " << word->second << " " << total[tag->first] << endl;
      }
      else if(tag->first == "NN" && word->first == "spread"){
        cout << tag->first << emission_prob[current_word_id][current_tag_id] << " " << word->second << total[tag->first] << endl;
      }
    }
    regex uniq("\\$||#||,||END");
    //cout << tag->first << total[tag->first] << " " << word_id.size() << " " << smt_count / (total[tag->first] + word_id.size() * smt_count) << endl;
    if(!regex_match(tag->first, uniq)){
      emission_prob[word_id["<UNK>"]][current_tag_id] = smt_count / (total[tag->first] + word_id.size() * smt_count);
    }
    else cout << tag->first  << endl;
  }
}

void viterbi(vector<string> &input, vector<string> &answer, vector<vector<double> > &transition_prob, vector<vector<double> > &emission_prob){
  vector<vector<double> > prob_table(input.size());
  vector<vector<int> > backpointer(input.size());
  double max = 0, tmp_prob, e_prob;
  int max_index;
  for(int i=0; i<input.size(); i++){
    prob_table[i] = *(new vector<double>(tag_id.size()));
    backpointer[i] = *(new vector<int>(word_id.size()));
  }
  for(int i=0; i<tag_id.size(); i++){
    if(word_id.find(input[0]) == word_id.end() || emission_prob[word_id[input[0]]][i] == 0){
      e_prob = emission_prob[word_id["<UNK>"]][i];
    }
    else{
      e_prob = emission_prob[word_id[input[0]]][i];
    }
    prob_table[0][i] = transition_prob[tag_id["START"]][i] * e_prob;
    //cout << transition_prob[tag_id["START"]][i] << " " << e_prob << " " << get_string(i, tag_id) << " "  << prob_table[0][i] << endl;
    backpointer[0][i] = 0;
  }
  for(int i=1; i<input.size(); i++){
    for(int j=0; j<tag_id.size(); j++){
      for(int k=0; k<tag_id.size(); k++){
        if(word_id.find(input[i]) == word_id.end() || emission_prob[word_id[input[i]]][j] == 0){
          e_prob = emission_prob[word_id["<UNK>"]][j];
        }
        else{
          e_prob = emission_prob[word_id[input[i]]][j];
        }
        if((tmp_prob = prob_table[i-1][k] * transition_prob[k][j] * e_prob) > max) {
          max = tmp_prob;
          max_index = k;
        }
      }
      prob_table[i][j] = max;
      backpointer[i][j] = max_index;
      max = 0;
    }
  }


  int last_tag = 0;
  int last_index = input.size()-1;
  double max_prob = prob_table[last_index][last_tag] * transition_prob[last_tag][tag_id["END"]];
  for(int i=1; i<tag_id.size(); i++){
    tmp_prob = prob_table[last_index][i] * transition_prob[i][tag_id["END"]];
    if(max_prob < tmp_prob) {
      last_tag = i;
      max_prob = tmp_prob;
    }
  }
  
  //print result
  vector<vector<int> > backtrace(K);
  int next_tag = last_tag;
  for(int i=0; i<K; i++){
    for(int index=last_index; index >=0; index--){
      backtrace[i].push_back(next_tag);
      next_tag = backpointer[index][next_tag];
    }
  }
  for(int i = 0; i < input.size(); i++){
    //cout << input[i] << " " << answer[i] << " ";
    for(int j = 0; j < K; j++){
      if(backtrace[j].back() == tag_id[answer[i]]){
        //cout << get_string(backtrace[j].back(), tag_id) << " ";
      }
      else{
        cout << input[i] << " " << answer[i] << " ";
        cout << "*" << get_string(backtrace[j].back(), tag_id) << " ";
        cout << error[j]++ << endl;
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
  //vector<vector<int> > transition_count(t_size);
  //vector<vector<int> > emission_count(w_size);
  vector<vector<double> > transition_prob(t_size);
  vector<vector<double> > emission_prob(w_size);
  make_HMM_graph(transition_prob, emission_prob);
  int total_words;
  vector<string> sentence, answer;
  while(getline(ifs_t, test_input)){
    if (test_input.length() == 0){
      //cout << input[0] << endl;
      viterbi(sentence, answer, transition_prob, emission_prob);
      sentence.clear();
      answer.clear();
    }
    else{
      total_words++;
      int index = test_input.find(" ", 0);
      int index2 = test_input.find(" ", index+1);
      answer.push_back(test_input.substr(index+1, index2-index-1));
      sentence.push_back(test_input.substr(0, index));
    }
  }
  cout << "accurate" << endl;
  for(int i = 0; i < K; i++){
    cout << i+1 << ". " << error[i] << " " << total_words << " " << (float)(total_words-error[i])/total_words << endl;
  }
  /*debug
  cout << transition_prob[tag_id["VBZ"]][tag_id["NN"]] << " " << emission_prob[word_id["spread"]][tag_id["NN"]] << endl;
  cout << transition_prob[tag_id["VBZ"]][tag_id["VBN"]] << " " << emission_prob[word_id["spread"]][tag_id["VBN"]] << endl;
  for(int i=0; i<tag_id.size();i++){
   cout << get_string(i, tag_id) <<  " " << emission_prob[word_id["<UNK>"]][i] << endl;
  }
  */
}

