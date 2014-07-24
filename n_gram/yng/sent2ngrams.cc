#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

int main (int argc, char** argv) {
  if (argc < 2)
    { std::fprintf (stdout, "Usage: %s N", argv[0]); std::exit (1); }
  const int N = std::strtol (argv[1], NULL, 10);
  
  char line[1 << 21];
  std::vector <const char*> words (N - 1, "<s>");
  while (std::fgets (line, 1 << 21, stdin) != NULL) {
    words.resize (N - 1);
    for (char* p = line; *p != '\n'; ++p) {
      words.push_back (p);
      while (*p != ' ') ++p; *p ='\0'; // faster strtok
    }
    words.push_back ("</s>");
    for (int i = N - 1; i < words.size (); ++i)
      for (int n = 1; n <= N; ++n) {
        for (int j = i - (n - 1); j <= i; ++j) {
          std::fprintf (stdout, "%s", words[j]);
          if (j != i) std::fprintf (stdout, " ");
        }
        std::fprintf (stdout, "\n");
      }
  }
}
