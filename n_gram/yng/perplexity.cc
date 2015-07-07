#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <vector>
#include <tr1/unordered_set>
#include "cedarpp.h"

int main (int argc, char** argv) {
  if (argc < 3)
    { std::fprintf (stderr, "Usage: %s\n", argv[0]); std::exit (1); }
  enum smoothing_t { NONE = 0, LAPLACE = 1 };
  
  typedef cedar::da <int, -1, -2, true>  trie_t;
  trie_t trie;
  trie_t vocabulary;
  
  char line[1024];
  FILE* fp = std::fopen (argv[1], "r");
  size_t N = 0; // N of N-grams
  size_t C = 0; // # tokens (for unigram probability)
  size_t V = 0; // # vocabulary (exclude '<s>')
  while (std::fgets (line, 1 << 21, fp) != NULL) {
    char* p = 0;
    int count = static_cast <int> (std::strtol (line, &p, 10));
    *p = ' ';
    char* head  = p + 1;                               // start of N-gram
    char* tail  = head + std::strlen (head) - 1;       // end of N-gram
    char* tail_ = tail; while (*tail_ != ' ') --tail_; // end of N-1-gram
    // check N in N-grams
    if (! N)
      for (char* q = p; q != tail; ++q) 
        if (*q == ' ') ++N;
    // register count of N-gram
    trie.update (head, tail - head)  = count;
    // register count of N-1-gram (denominator)
    if (N == 1)
      C += count;
    else
      trie.update (head, tail_ - head) += count;
    // register a (last) word to vocabulary
    char* head_ = tail_ + 1;
    int &id = vocabulary.update (head_, tail - head_);
    if (! id) id = ++V;
  }
  std::fprintf (stderr, "# %ld-grams: %ld; # vocabulary: %ld\n", N, trie.num_keys (), V);
  //
  smoothing_t type = static_cast <smoothing_t> (std::strtol (argv[2], NULL, 10));
  //
  std::vector <const char*> words (N - 1, "<s>");
  size_t sum  = 0;
  double logP = 0;
  while (std::fgets (line, 1 << 21, stdin) != NULL) {
    words.resize (N - 1);
    for (char* p = line; *p != '\n'; ++p) {
      words.push_back (p);
      while (*p != ' ') ++p; *p = '\0';
    }
    words.push_back ("</s>");
    for (size_t i = N - 1; i < words.size (); ++i) {
      cedar::npos_t from = 0;
      size_t pos (0);
      int n (0), m (0);
      if (N == 1) {
        n = C;
        m = trie.traverse (words[i], from, pos = 0);
      } else {
        size_t j = i - (N - 1);
        // count for N-1-gram (traversing one word by one)
        for (; j < i; ++j)
          if ((n = trie.traverse (words[j], from, pos = 0)) == -2 ||
              trie.traverse (" ", from, pos = 0, 1) == -2)
            break;
        // count for N-gram
        if (j == i && n > 0)
          m = trie.traverse (words[j], from, pos = 0);
      }
      m = std::max (m, 0);
      n = std::max (n, 0);
      double P = 0.0;
      switch (type) {
        case NONE:    P = m * 1.0 / n; break;
        case LAPLACE: P = (m + 1.0) / (n + V + 1.0); break;
      }
      std::fprintf (stdout, "%s\t%f\n", words[i], std::exp (- std::log (P)));
      logP += std::log (P);
      ++sum;
    }
    std::fprintf (stdout, "\n");
  }
  std::fprintf (stderr, "perplexity: %f\n", std::exp (- logP / sum));
  return 0;
}
