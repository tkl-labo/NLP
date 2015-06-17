// spell.cc -- a simple spell checker based on minmum edit distance
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>

inline size_t ins_cost (int c) { return 1; }
inline size_t sub_cost (int c1, int c2) { return c1 == c2 ? 0 : 2; }
inline size_t del_cost (int c) { return 1; }


size_t min_edit_dist (const std::vector <int>& target,
                      const std::vector <int>& source) {
  const size_t n = target.size ();
  const size_t m = source.size ();
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

// UTF8 <-> Unicode
class encoder {
private:
  std::vector <int>  unicode;
  std::vector <char> utf8;
public:
  encoder () {}
  const std::vector <int>& encode (const char* str) { // UTF8 -> Unicode
    unicode.clear ();
    for (const char* p = str; *p; ++p) { // https://ja.wikipedia.org/wiki/UTF-8
      int u (0), i (0);
      if (*p & 0x80) // 2 - 6 bytes
        do {
          u <<= 6;
          u += p[++i] & 0x3f;
        } while (*p & (0x40 >> i));
      u += (*p & (0x7f >> i)) << (i * 6);
      p += i;
      unicode.push_back (u);
    }
    return unicode;
  };
  const char* decode (const std::vector <int>& str) {
    utf8.clear ();
    for (std::vector <int>::const_reverse_iterator it = str.rbegin ();
         it != str.rend (); ++it)
      if (*it >> 7) { // 2 - 6 bytes
        int i = 0;
        do
          utf8.push_back (((*it >> (i++ * 6)) & 0x3f) | 0x80);
        while (*it >> (i * 6 + 6 - i));
        utf8.push_back ((*it >> (i * 6)) | (0x7f << (7 - i)));
      } else // 1 byte; ascii
        utf8.push_back (*it);
    std::reverse (utf8.begin (), utf8.end ());
    utf8.push_back ('\0');
    return &utf8[0];
  };
};

int main (int argc, char** argv) {
  if (argc >= 3)
    { std::fprintf (stderr, "Usage: %s [dict]\n", argv[0]); std::exit (1); }

  // read dict
  FILE* fp = std::fopen (argc >= 2 ? argv[1] : "/usr/share/dict/words", "r");
  if (! fp)
    { std::fprintf (stderr, "no such file: %s\n", argv[1]); std::exit (1); }
  //
  encoder encoder;
  std::vector <std::vector <int> > words;
  char line[1 << 21];
  while (std::fgets (line, 1 << 21, fp) != 0) {
    line[std::strlen (line) - 1] = '\0';
    words.push_back (encoder.encode (line));
  }
  //
  typedef std::vector <std::pair <size_t, size_t> >  result_t;
  result_t results (words.size ());
  // loop
  std::fprintf (stderr, "> ");
  while (std::fgets (line, 1024, stdin) != NULL) {
    line[std::strlen (line) - 1] = '\0';
    for (size_t i = 0; i < words.size (); ++i)
      results[i] = result_t::value_type (min_edit_dist (encoder.encode (line),
                                                        words[i]), i);
    std::sort (results.begin (), results.end ());
    for (result_t::iterator it = results.begin (); it != results.end (); ++it) {
      std::fprintf (stdout, "\t%s: %ld\n",
                    encoder.decode (words[it->second]), it->first);
      std::fprintf (stdout, "-- stop enumeration? [y]: ");
      std::fgets (line, 1024, stdin);
      if (std::feof (stdin) || line[0] == 'y') break;
    }
    std::fprintf (stderr, "\n> ");
  }
}
