// tagger.cc -- implementation of viterbi algorithm for first-order HMM
#include <sys/time.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <climits>
#include <limits>
#include <vector>
#include <algorithm>
#include <cedarpp.h>

typedef cedar::da <int>  sbag_t;

struct hmm_t {
  std::vector <double> init;
  std::vector <std::vector <double> > transition; // tag  <- tag (pre)
  std::vector <std::vector <double> > emission;   // word <- tag
  void add_tag () { // register unseen tags
    init.push_back (0.0);
    for (size_t i = 0; i < transition.size (); ++i)
      transition[i].push_back (0.0);
    transition.push_back (std::vector <double> (transition.size () + 1, 0.0));
    for (size_t i = 0; i < emission.size (); ++i)
      emission[i].push_back (std::log (1.0 / emission.size ()));
  }
  int num_tags () const { return static_cast <int> (transition.size ()); }
};

void read_model (char* model, sbag_t& tag2id, sbag_t& word2id, hmm_t& hmm) {
  int flag = 0;
  int num_tags  = 0;
  int num_words = 1; // 0 is reserved for unknown words
  FILE* fp = std::fopen (model, "r");
  char line[1024];
  while (std::fgets (line, 1024, fp) != NULL) {
    size_t len = std::strlen (line) - 1;
    char* p = line;
    if (len)
      switch (flag) {
      case 0: tag2id.update  (p, len) = num_tags++;  break; // tag  -> id
      case 1: word2id.update (p, len) = num_words++; break; // word -> id
      case 2: // init
        for (; *p != '\n'; ++p)
          hmm.init.push_back (std::log (std::strtod (p, &p)));
        break;
      case 3: // tag <- tag (prev); // transpose for cache access
        if (hmm.transition.empty ())
          hmm.transition.resize (num_tags, std::vector <double> ());
        for (size_t i = 0; *p != '\n'; ++i, ++p)
          hmm.transition[i].push_back (std::log (std::strtod (p, &p)));
        break;
      case 4: // word <- tag
        hmm.emission.push_back (std::vector <double> ());
        hmm.emission.back ().reserve (num_tags);
        for (; *p != '\n'; ++p)
          hmm.emission.back ().push_back (std::log (std::strtod (p, &p)));
        break;
    } else
      ++flag;
  }
  std::fclose (fp);
}

void viterbi (const std::vector <int>& words, const hmm_t& hmm,
              std::vector <int>& tags) {
  size_t n = words.size ();
  const size_t m = hmm.num_tags ();
  // avoid memory reallocation for viterbi matrix
  static std::vector <std::vector <double> > log_prob;
  static std::vector <std::vector <int> > bptr;
  if (log_prob.size () < n) {
    log_prob.resize (n, std::vector <double> (m, 0.0));
    bptr.resize (n, std::vector <int> (m, -1));
  }
  for (size_t i = 0; i < log_prob.size () && log_prob[i].size () < m; ++i) {
    log_prob[i].resize (m, 0.0);
    bptr[i].resize (m, -1);
  }
  tags.resize (n);
  // initialize
  for (size_t j = 0; j < m; ++j)
    log_prob[0][j] = hmm.init[j] + hmm.emission[words[0]][j];
  for (size_t i = 1; i < n; ++i) // for each word
    for (size_t j = 0; j < m; ++j) { // for each tag
      size_t max_k = 0;
      double max_log_prob = - std::numeric_limits <double>::infinity ();
      for (size_t k = 0; k < m; ++k) {
        double val = log_prob[i-1][k] + hmm.transition[j][k] + hmm.emission[words[i]][j];
        if (max_log_prob <= val)
          max_log_prob = val, max_k = k;
      }
      log_prob[i][j] = max_log_prob;
      bptr[i][j] = max_k;
    }
  int tag = std::max_element (log_prob[n-1].begin (), log_prob[n-1].end ()) - log_prob[n-1].begin ();
  do {
    tags[--n] = tag;
    tag = bptr[n][tag];
  } while (n);
}

int main (int argc, char** argv) {
  if (argc < 2) {
    std::fprintf (stderr, "Usage: %s model < test\n", argv[0]);
    std::exit (1);
  }
  struct timeval start, end;
  // read model
  hmm_t hmm;
  sbag_t tag2id, word2id;
  std::fprintf (stderr, "reading model: ");
  gettimeofday (&start, 0);
  read_model (argv[1], tag2id, word2id, hmm);
  gettimeofday (&end, 0);
  std::fprintf (stderr, "%.3fs\n", end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1e-6);
  // tagging
  std::fprintf (stderr, "tagging words: ");
  gettimeofday (&start, 0);
  // some temporary variables
  int corr (0), incorr (0);
  size_t num_sent = 0;
  std::vector <int> words, tags, tags_gold;
  char line[1024];
  while (std::fgets (line, 1024, stdin) != NULL) {
    if (line[0] == '\n') {
      viterbi (words, hmm, tags);
      for (size_t i = 0; i < tags.size (); ++i)
        if (tags[i] == tags_gold[i]) ++corr; else ++incorr;
      words.clear ();
      tags.clear ();
      tags_gold.clear ();
      if (++num_sent % 1000 == 0)
        std::fprintf (stderr, ".");
      continue;
    }
    char* p = line;
    char* word = p; while (*p != ' ') ++p;
    int id = word2id.exactMatchSearch <int> (word, p - word);
    words.push_back (id == -1 ? 0 : id);
    char* tag = ++p; while (*p != ' ') ++p;
    id = tag2id.exactMatchSearch <int> (tag, p - tag);
    if (id == -1) { // unknown tag found
      *p = '\0'; std::fprintf (stderr, "unknown tag found: %s\n", tag);
      hmm.add_tag ();
      id = tag2id.update (tag, p - tag) = hmm.num_tags () - 1;
    }
    tags_gold.push_back (id);
  }
  gettimeofday (&end, 0);
  std::fprintf (stderr, "%.3fs\n", end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1e-6);
  std::fprintf (stderr, "# sentences: %ld\n", num_sent);
  std::fprintf (stderr, "acc. %.4f (corr %d) (incorr %d)\n",
                corr * 1.0 / (corr + incorr), corr, incorr);
}
