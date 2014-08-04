// sent2ngrams_trie.cc -- enumerate and count n-grams from given sentences
//  * compressed n-grams are stored in a prefix trie
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <algorithm>
#include "cedarpp.h"

static const size_t KEY_SIZE = 8;
typedef cedar::da <int> trie_t;
typedef unsigned char uchar;

// integer <-> byte encoded compact string
class byte_encoder {
public:
  byte_encoder () : _len (0), _key () {}
  byte_encoder (size_t i) : _len (0), _key () { encode (i); }
  size_t encode  (size_t i, uchar* const key_) const {
    size_t len_ = 0;
    for (key_[len_] = (i & 0x7f); i >>= 7; key_[++len_] = (i & 0x7f))
      key_[len_] |= 0x80;
    return ++len_;
  }
  void encode (const size_t i) { _len = encode (i, _key); }
  size_t decode (size_t& i, const uchar* const key_) const {
    size_t b (0), len_ (0);
    for (i = key_[0] & 0x7f; key_[len_] & 0x80; i += (key_[len_] & 0x7fu) << b)
      ++len_, b += 7;
    return ++len_;
  }
  size_t      len () { return _len; }
  const char* key () { return reinterpret_cast <const char*> (&_key[0]); }
private:
  size_t _len;
  uchar  _key[KEY_SIZE];
};

// raw string (word) <-> integer
class sbag_t {
private:
  size_t _num_v;
  trie_t _word2id;
  std::vector <std::string> _id2word;
public:
  sbag_t () : _num_v (2), _word2id (), _id2word () {
    _id2word.push_back (""); // dummy
    _id2word.push_back ("<s>");
    _id2word.push_back ("</s>");
  }
  size_t to_id (const char* word, size_t len) {
    int& n = _word2id.update (word, len);
    if (! n) {
      n = ++_num_v;
      _id2word.push_back (std::string (word, len));
    }
    return n;
  }
  const char* to_str (size_t id) const { return _id2word[id].c_str (); }
};

int main (int argc, char** argv) {
  if (argc < 2)
    { std::fprintf (stdout, "Usage: %s N", argv[0]); std::exit (1); }
  const int N = std::strtol (argv[1], NULL, 10);

  sbag_t sbag;
  trie_t ngrams;
  //
  char line[1 << 21];
  
  size_t num_v = 2; // 1 and 2 are reserved for <s> and </s>
  for (std::vector <size_t> words (N - 1, 1);
       std::fgets (line, 1 << 21, stdin) != NULL; words.resize (N - 1)) {
    for (char* p = line; *p != '\n'; ++p) {
      char* word = p;
      while (*p != ' ') ++p; // faster strtok
      words.push_back (sbag.to_id (word, p - word));
    }
    words.push_back (2);
    for (int i = 0; i < words.size (); ++i) {
      for (size_t n = 1; n <= N; ++n) {
        if (i + n < N) continue;
        cedar::npos_t from = 0;
        size_t pos = 0;
        int* v = 0;
        for (int j (i), k (std::min (i + n, words.size ())); j < k; ++j) {
          byte_encoder encoder (words[j]);
          v = &ngrams.update (encoder.key (), from, pos = 0, encoder.len ());
        }
        if (v) *v += 1;
      }
    }
  }
  // output count
  int n = static_cast <int> (ngrams.num_keys ());
  trie_t::result_triple_type* result = new trie_t::result_triple_type[n];
  ngrams.dump (result, n);
  byte_encoder encoder;
  for (int i = 0; i < n; ++i) {
    if (result[i].value == 0) continue;
    char suffix[1024];
    ngrams.suffix (suffix, result[i].length, result[i].id);
    // std::fprintf (stdout, "%d\t%s\n", result[i].value, suffix);
    std::fprintf (stdout, "%d\t", result[i].value);
    for (size_t len = 0; len < result[i].length; ) {
      size_t j = 0;
      len += encoder.decode (j, reinterpret_cast <const uchar*> (&suffix[len]));
      std::fprintf (stdout, "%s", sbag.to_str (j));
      std::fprintf (stdout, len < result[i].length ? " " : "\n");
    }
  }
}
