// tagger.cc -- find the best tag sequences using Viterbi algorithm
#include <sys/time.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <vector>
#include <string>
#include <algorithm>
#include <bnt.h>

struct hmm_t {
  std::vector <double> init;
  std::vector <std::vector <double> > transition; // tag  <- tag (pre)
  std::vector <std::vector <double> > emission;   // word <- tag
  int num_tags () const { return static_cast <int> (transition.size ()); }
};

void read_model (char* model, std::vector <std::string>& id2tag, sbag_t& tag2id, sbag_t& word2id, hmm_t& hmm) {
  int flag = 0;
  int num_tags  = 0;
  int num_words = NUM_UNK_TYPE; // reserved for unknown words
  FILE* fp = std::fopen (model, "r");
  char line[1 << 21];
  while (std::fgets (line, 1 << 21, fp) != NULL) {
    size_t len = std::strlen (line) - 1;
    char* p = line;
    if (len)
      switch (flag) {
      case 0: // tag  -> id
        id2tag.push_back (std::string (p, len));
        tag2id.update (p, len) = num_tags++;
        break;
      case 1: word2id.update (p, len) = num_words++; break; // word -> id
      case 2: // init
        for (; *p != '\n'; ++p)
          hmm.init.push_back (std::log (std::strtod (p, &p)));
        break;
      case 3: // tag <- tag (prev); // transpose for better caching
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

template <typename T>
struct cmp_t
{ bool operator() (const T& a, const T& b) const { return a.first > b.first; } };

void viterbi (const std::vector <int>& words, const hmm_t& hmm,
              std::vector <int>& tags) {
  size_t n = words.size ();
  const size_t m = hmm.num_tags ();
  // avoid memory reallocation for viterbi matrix
  static std::vector <std::vector <std::pair <double, size_t> > > log_prob;
  static std::vector <std::vector <size_t> > bptr;
  if (log_prob.size () < n) {
    for (size_t i = 0; i < log_prob.size (); ++i) {
      log_prob[i].resize (m);
      bptr[i].resize (m, 0);
    }
    log_prob.resize (n, std::vector <std::pair <double, size_t> > (m));
    bptr.resize (n, std::vector <size_t> (m, 0));
    tags.resize (n);
  }
  // initialize
  for (size_t j = 0; j < m; ++j)
    log_prob[0][j] = std::pair <double, size_t> (hmm.init[j] + hmm.emission[words[0]][j], j);
  for (size_t i = 1; i < n; ++i) { // for each word
    std::sort (log_prob[i-1].begin (), log_prob[i-1].end (),
               cmp_t <std::pair <double, size_t> > ());
    for (size_t j = 0; j < m; ++j) { // for each tag
      size_t max_k = 0;
      double max_log_prob = - std::numeric_limits <double>::infinity ();
      if (std::fpclassify (hmm.emission[words[i]][j]) != FP_INFINITE)
#ifdef USE_BEAM_SIZE
        for (size_t k = 0; k < USE_BEAM_SIZE; ++k)
#else
        for (size_t k = 0; k < m; ++k)
#endif
          {
            if (max_log_prob >= log_prob[i-1][k].first) break;
            const int tag = log_prob[i-1][k].second;
            double val = log_prob[i-1][k].first + hmm.transition[j][tag];
            if (max_log_prob <= val)
              max_log_prob = val, max_k = k;
          }
      log_prob[i][j] = std::pair <double, size_t> (max_log_prob + hmm.emission[words[i]][j], j);
      bptr[i][j] = max_k;
    }
  }
  for (size_t k = std::max_element (log_prob[n-1].begin (), log_prob[n-1].end ())
                  - log_prob[n-1].begin (); n--; ) {
    const int tag = log_prob[n][k].second;;
    tags[n] = tag;
    k = bptr[n][tag];
  }
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
  std::vector <std::string> id2tag;
  std::fprintf (stderr, "reading model: ");
  gettimeofday (&start, 0);
  read_model (argv[1], id2tag, tag2id, word2id, hmm);
  gettimeofday (&end, 0);
  std::fprintf (stderr, "%.3fs\n", end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1e-6);
  // tagging
  std::fprintf (stderr, "tagging words: ");
  gettimeofday (&start, 0);
  // some temporary variables
  int stat[4];
  for (size_t i = 0; i < 4; ++i) stat[i] = 0;
  size_t num_sents (0), num_tokens (0);
  std::vector <int> words, tags, tags_gold;
  std::vector <std::string> words_str;
  char line[1 << 21];
  while (std::fgets (line, 1 << 21, stdin) != NULL) {
    if (line[0] == '\n') {
      viterbi (words, hmm, tags);
      // collect statistics and output
      for (size_t i = 0; i < tags_gold.size (); ++i) {
        int bit = tags[i] == tags_gold[i] ? 2 : 0;
        bit |= words[i] >= NUM_UNK_TYPE ? 1 : 0;
        ++stat[bit];
#ifndef NDEBUG
        if (tags[i] != tags_gold[i]) std::fprintf (stdout, "\x1b[31m");
        std::fprintf (stdout, "%s/%s/%s ", words_str[i].c_str (), id2tag[tags_gold[i]].c_str (), id2tag[tags[i]].c_str ());
        if (tags[i] != tags_gold[i]) std::fprintf (stdout, "\x1b[39m");
#endif
      }
#ifndef NDEBUG
      std::fprintf (stdout, "\n");
      words_str.clear ();
#endif
      num_tokens += words.size ();
      words.clear ();
      tags_gold.clear ();
      if (++num_sents % 1000 == 0) std::fprintf (stderr, ".");
      continue;
    }
    char* p = line;
    char* word = p; while (*p != ' ') ++p;
    int id = word2id.exactMatchSearch <int> (word, p - word);
#ifndef NDEBUG
    words_str.push_back (std::string (word, p - word));
#endif
    words.push_back (id != -1 ? id : unk_classify (word, p - 1));
    char* tag = ++p; while (*p != ' ') ++p;
    if ((id = tag2id.exactMatchSearch <int> (tag, p - tag)) == -1) {
      // unknown tag found
      *p = '\0'; std::fprintf (stderr, "unknown tag found: %s\n", tag);
      id2tag.push_back (std::string (tag, p - tag));
      id = tag2id.update (tag, p - tag) = id2tag.size () - 1;
    }
    tags_gold.push_back (id);
  }
  gettimeofday (&end, 0);
  //
  std::fprintf (stderr, "%.3fs\n", end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1e-6);
  std::fprintf (stderr, "# sentences: %ld\n", num_sents);
  std::fprintf (stderr, "acc. %.4f (corr %d) (incorr %d)\n",
                (stat[2] + stat[3]) * 1.0 / num_tokens,
                stat[2] + stat[3], stat[0] + stat[1]);
  std::fprintf (stderr, "  (seen)   %.4f (corr %d) (incorr %d)\n",
                stat[3] * 1.0 / (stat[1] + stat[3]), stat[3], stat[1]);
  std::fprintf (stderr, "  (unseen) %.4f (corr %d) (incorr %d)\n",
                stat[2] * 1.0 / (stat[0] + stat[2]), stat[2], stat[0]);
}
