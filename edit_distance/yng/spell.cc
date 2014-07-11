// spell.cc -- a simple spell checker based on minmum edit distance
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>

inline size_t ins_cost (char c) { return 1; }
inline size_t sub_cost (char c1, char c2) { return c1 == c2 ? 0 : 2; }
inline size_t del_cost (char c) { return 1; }

size_t min_edit_dist (const char* target, const char* source) {
  const size_t n = std::strlen (target);
  const size_t m = std::strlen (source);
  //
  // avoid memory reallocation for distance matrix
  static std::vector <std::vector <size_t> > dist;
  if (dist.size () < n+1 || dist.front ().size () < m+1)
    dist.clear ();
  if (dist.empty ())
    dist.resize (n+1, std::vector <size_t> (m+1, 0));
  //
  // initialize
  dist[0][0] = 0;
  for (size_t i = 1; i <= n; ++i)
    dist[i][0] = dist[i-1][0] + ins_cost (target[i-1]);
  for (size_t j = 1; j <= m; ++j)
    dist[0][j] = dist[0][j-1] + del_cost (source[j-1]);
  //
  // execute dynamic programming
  for (size_t i = 1; i <= n; ++i)
    for (size_t j = 1; j <= m; ++j)
      dist[i][j] = dist[i-1][j-1] + sub_cost (target[i-1], source[j-1]),
      dist[i][j] = std::min (dist[i][j], dist[i-1][j] + ins_cost (target[i-1])),
      dist[i][j] = std::min (dist[i][j], dist[i][j-1] + del_cost (source[j-1]));
  return dist[n][m];
}

int main (int argc, char** argv) {
  if (argc >= 3)
    { std::fprintf (stderr, "Usage: %s [dict]\n", argv[0]); std::exit (1); }
  
  // read dict
  FILE* fp = std::fopen (argc >= 2 ? argv[1] : "/usr/share/dict/words", "r");
  if (! fp)
    { std::fprintf (stderr, "no such file: %s\n", argv[1]); std::exit (1); }
  if (std::fseek (fp, 0, SEEK_END) != 0) return -1;
  const size_t size = static_cast <size_t> (std::ftell (fp));
  if (std::fseek (fp, 0, SEEK_SET) != 0) return -1;
  char* data = new char[size];
  if (size != std::fread (data, sizeof (char), size, fp)) return -1;
  //
  std::vector <const char*> words;
  for (char* start (data), *end (data), *tail (data + size);
       end != tail; start = ++end) {
    while (*end != '\n') ++end; *end = '\0';
    words.push_back (start);
  }
  //
  typedef std::vector <std::pair <size_t, size_t> >  result_t;
  result_t results (words.size ());
  // loop
  char line[1024];
  std::fprintf (stderr, "> ");
  while (std::fgets (line, 1024, stdin) != NULL) {
    line[std::strlen (line) - 1] = '\0';
    for (size_t i = 0; i < words.size (); ++i)
      results[i] = result_t::value_type (min_edit_dist (line, words[i]), i);
    std::sort (results.begin (), results.end ());
    for (result_t::iterator it = results.begin (); it != results.end (); ++it) {
      std::fprintf (stdout, "\t%s: %ld\n", words[it->second], it->first);
      std::fprintf (stdout, "-- stop enumeration? [y]: ");
      std::fgets (line, 1024, stdin);
      if (std::feof (stdin) || line[0] == 'y') break;
    }
    std::fprintf (stderr, "\n> ");
  }
}
