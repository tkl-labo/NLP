#include <cstdio>
#include <cstring>

int main () {
  char line[1 << 21];
  while (std::fgets (line, 1 << 21, stdin) != NULL) {
    if (std::strcmp (line, "EOS\n") == 0)
      std::fprintf (stdout, "\n");
    else {
      char* p = line;
      while (*p != '\t') ++p;
      std::fwrite (line, sizeof (char), p - line, stdout);
      std::fputs (" ", stdout);
    }
  }
  return 0;
}
