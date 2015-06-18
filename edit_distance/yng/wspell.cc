// spell.cc -- a simple spell checker based on minmum edit distance
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cwchar>
#include <clocale>
#include <vector>
#include <algorithm>

inline size_t ins_cost (int c) { return 1; }
inline size_t sub_cost (int c1, int c2) { return c1 == c2 ? 0 : 2; }
inline size_t del_cost (int c) { return 1; }


size_t min_edit_dist (const std::vector <wchar_t>& target,
                      const std::vector <wchar_t>& source) {
  const size_t n = std::wcslen (&target[0]);
  const size_t m = std::wcslen (&source[0]);
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
  //
  std::setlocale (LC_CTYPE, "ja_JP.UTF-8");
  // read dict
  FILE* fp = std::fopen (argc >= 2 ? argv[1] : "/usr/share/dict/words", "r");
  if (! fp)
    { std::fprintf (stderr, "no such file: %s\n", argv[1]); std::exit (1); }
  //
  std::vector <std::vector <wchar_t> > words;
  char line[1 << 21];
  std::vector <wchar_t> w;
  while (std::fgets (line, 1 << 21, fp) != 0) {
    size_t len = std::strlen (line) - 1;
    line[std::strlen (line) - 1] = '\0';
    w.resize (len + 1);
    w.resize (std::mbstowcs (&w[0], line, len + 1) + 1);
    words.push_back (w);
  }
  //
  typedef std::vector <std::pair <size_t, size_t> >  result_t;
  result_t results (words.size ());
  // loop
  std::fprintf (stderr, "> ");
  while (std::fgets (line, 1024, stdin) != NULL) {
    size_t len = std::strlen (line) - 1;
    line[len] = '\0';
    w.resize (len + 1);
    w.resize (std::mbstowcs (&w[0], line, len + 1) + 1);
    for (size_t i = 0; i < words.size (); ++i)
      results[i] = result_t::value_type (min_edit_dist (w, words[i]), i);
    std::sort (results.begin (), results.end ());
    for (result_t::iterator it = results.begin (); it != results.end (); ++it) {
      char ret[1 << 21];
      std::wcstombs (&ret[0], &words[it->second][0], std::wcslen (&words[it->second][0]) * MB_CUR_MAX + 1);
      std::fprintf (stdout, "\t%s: %ld\n", ret, it->first);
      std::fprintf (stdout, "-- stop enumeration? [y]: ");
      std::fgets (line, 1024, stdin);
      if (std::feof (stdin) || line[0] == 'y') break;
    }
    std::fprintf (stderr, "\n> ");
  }
}
