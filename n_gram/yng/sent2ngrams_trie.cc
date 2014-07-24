#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>
#include "cedarpp.h"

int main (int argc, char** argv) {
  if (argc < 2)
    { std::fprintf (stdout, "Usage: %s N", argv[0]); std::exit (1); }
  const int N = std::strtol (argv[1], NULL, 10);

  typedef cedar::da <int> trie_t;
  trie_t trie;
  
  char line[1 << 21];
  std::vector <const char*> words (N - 1, "<s>");
  while (std::fgets (line, 1 << 21, stdin) != NULL) {
    words.resize (N - 1);
    for (char* p = line; *p != '\n'; ++p) {
      words.push_back (p);
      while (*p != ' ') ++p; *p ='\0'; // faster strtok
    }
    words.push_back ("</s>");
    for (int i = 0; i < words.size (); ++i)
      for (size_t n = 1; n <= N; ++n) {
        if (i + n < N) continue;
        cedar::npos_t from = 0;
        size_t pos = 0;
        int* v = 0;
        for (int j (i), k (std::min (i + n, words.size ())); j < k; ++j) {
          v = &trie.update (words[j], from, pos = 0, std::strlen (words[j]));
          if (j != k - 1) trie.update (" ", from, pos = 0, 1);
        }
        if (v) *v += 1;
      }
  }
  // output count
  int n = static_cast <int> (trie.num_keys ());
  trie_t::result_triple_type* result = new trie_t::result_triple_type[n];
  trie.dump (result, n);
  for (int i = 0; i < n; ++i) {
    if (result[i].value == 0) continue;
    char suffix[1024];
    trie.suffix (suffix, result[i].length, result[i].id);
    std::fprintf (stdout, "%d\t%s\n", result[i].value, suffix);
  }
}
