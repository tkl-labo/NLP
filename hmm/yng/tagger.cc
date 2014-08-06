// tagger.cc -- implementation of viterbi algorithm for first-order HMM
#include <sys/time.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <vector>
#include <string>
#include <algorithm>
#include <cedarpp.h>

typedef cedar::da <int>  sbag_t;

enum unk_type { HYPHEN, CAP, CD, CDE, END_S, END_ED, END_EN, END_ING, END_ER, END_EST, END_NOUN, END_VERB, END_ADV, END_ADJ, OTHER, NUM_UNK_TYPE };

unk_type unk_classify (char* beg, char* end) {
  for (char* p = beg; p <= end; ++p) if (*p == '-') return HYPHEN;
  if (*beg >= 'A' && *beg <= 'Z') return CAP;
  if (*beg <= '9' && *beg >= '0' && *end <= '9' && *end >= '0') return CD;
  if (*beg <= '9' && *beg >= '0') return CDE;
  if (end - beg >= 3 && std::strncmp (end - 3, "less", 4) == 0) return END_ADJ;
  if (end - beg >= 3 && std::strncmp (end - 3, "ible", 4) == 0) return END_ADJ;
  if (end - beg >= 3 && std::strncmp (end - 3, "able", 4) == 0) return END_ADJ;
  if (end - beg >= 2 && std::strncmp (end - 2, "eer",  3) == 0) return END_NOUN;
  if (end - beg >= 2 && std::strncmp (end - 2, "ess",  3) == 0) return END_NOUN;
  if (end - beg >= 2 && std::strncmp (end - 2, "ion",  3) == 0) return END_NOUN;
  if (end - beg >= 2 && std::strncmp (end - 2, "age",  3) == 0) return END_NOUN;
  if (end - beg >= 2 && std::strncmp (end - 2, "est",  3) == 0) return END_EST;
  if (end - beg >= 2 && std::strncmp (end - 2, "ful",  3) == 0) return END_ADJ;
  if (end - beg >= 2 && std::strncmp (end - 2, "ous",  3) == 0) return END_ADJ;
  if (end - beg >= 2 && std::strncmp (end - 2, "ing",  3) == 0) return END_ING;
  if (end - beg >= 2 && std::strncmp (end - 2, "ate",  3) == 0) return END_VERB;
  if (end - beg >= 1 && std::strncmp (end - 1, "en",   2) == 0) return END_EN;
  if (end - beg >= 1 && std::strncmp (end - 1, "ed",   2) == 0) return END_ED;
  if (end - beg >= 1 && std::strncmp (end - 1, "al",   2) == 0) return END_ADJ;
  if (end - beg >= 1 && std::strncmp (end - 1, "ly",   2) == 0) return END_ADV;
  if (end - beg >= 1 && std::strncmp (end - 1, "er",   2) == 0) return END_ER;
  if (end - beg >= 1 && std::strncmp (end - 1, "an",   2) == 0) return END_ADJ;
  if (end - beg >= 1 && std::strncmp (end - 1, "ty",   2) == 0) return END_NOUN;
  if (*end == 's') return END_S;
  return OTHER;
}

struct hmm_t {
  std::vector <double> init;
  std::vector <std::vector <double> > transition; // tag  <- tag (pre)
  std::vector <std::vector <double> > emission;   // word <- tag
  int num_tags () const { return static_cast <int> (transition.size ()); }
};

void read_model (char* model, std::vector <std::string>& id2tag, sbag_t& tag2id, sbag_t& word2id, hmm_t& hmm) {
  int flag = 0;
  int num_tags  = 0;
  int num_words = NUM_UNK_TYPE; // 0-5 is reserved for unknown words
  FILE* fp = std::fopen (model, "r");
  char line[1 << 21];
  while (std::fgets (line, 1 << 21, fp) != NULL) {
    size_t len = std::strlen (line) - 1;
    char* p = line;
    if (len)
      switch (flag) {
      case 0: // tag  -> id
        id2tag.push_back (std::string (p, len));
        tag2id.update  (p, len) = num_tags++;
        break;
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
  static std::vector <std::pair <double, size_t> > beam;
  static std::greater <std::pair <double, size_t> >  cmp;
  if (log_prob.size () < n) {
    log_prob.resize (n, std::vector <double> (m, 0.0));
    bptr.resize (n, std::vector <int> (m, -1));
  }
  for (size_t i = 0; i < log_prob.size () && log_prob[i].size () < m; ++i) {
    log_prob[i].resize (m, 0.0);
    bptr[i].resize (m, -1);
  }
  beam.resize (m);
  tags.resize (n);
  // initialize
  for (size_t j = 0; j < m; ++j) {
    log_prob[0][j] = hmm.init[j] + hmm.emission[words[0]][j];
    beam[j] = std::pair <double, size_t> (log_prob[0][j], j);
  }
   std::sort (beam.begin (), beam.end (), cmp);
  for (size_t i = 1; i < n; ++i) { // for each word
    for (size_t j = 0; j < m; ++j) { // for each tag
      size_t max_k = m;
      double max_log_prob = - std::numeric_limits <double>::infinity ();
      for (size_t l = 0; l < m && max_log_prob < beam[l].first; ++l) {
        size_t k = beam[l].second;
        double val = log_prob[i-1][k] + hmm.transition[j][k];
        if (max_log_prob <= val)
          max_log_prob = val, max_k = k;
      }
      log_prob[i][j] = max_log_prob + hmm.emission[words[i]][j];
      bptr[i][j] = max_k;
    }
    for (size_t j = 0; j < m; ++j)
      beam[j] = std::pair <double, size_t> (log_prob[i][j], j);
    std::sort (beam.begin (), beam.end (), cmp);
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
  int seen_corr (0), seen_incorr (0), unseen_corr (0), unseen_incorr (0);
  size_t num_sent = 0;
  std::vector <int> words, tags, tags_gold;
  std::vector <std::string> words_str;
  char line[1024];
  while (std::fgets (line, 1024, stdin) != NULL) {
    if (line[0] == '\n') {
      viterbi (words, hmm, tags);
      for (size_t i = 0; i < tags.size (); ++i) {
        if (tags[i] == tags_gold[i]) {
          if (words[i] >= NUM_UNK_TYPE) ++seen_corr;    else ++unseen_corr;
#ifndef NDEBUG
          std::fprintf (stdout, "%s/%s/%s ", words_str[i].c_str (), id2tag[tags_gold[i]].c_str (), id2tag[tags[i]].c_str ());
#endif
        } else {
          if (words[i] >= NUM_UNK_TYPE) ++seen_incorr;  else ++unseen_incorr;
#ifndef NDEBUG
          std::fprintf (stdout, "\x1b[31m%s/%s/%s\x1b[39m ", words_str[i].c_str (), id2tag[tags_gold[i]].c_str (), id2tag[tags[i]].c_str ());
#endif
        }
      }
#ifndef NDEBUG
      std::fprintf (stdout, "\n");
#endif
      words_str.clear ();
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
    words_str.push_back (std::string (word, p - word));
    words.push_back (id != -1 ? id : unk_classify (word, p - 1));
    char* tag = ++p; while (*p != ' ') ++p;
    id = tag2id.exactMatchSearch <int> (tag, p - tag);
    if (id == -1) { // unknown tag found
      *p = '\0'; std::fprintf (stderr, "unknown tag found: %s\n", tag);
      id2tag.push_back (std::string (tag, p - tag));
      id = tag2id.update (tag, p - tag) = hmm.num_tags () - 1;
    }
    tags_gold.push_back (id);
  }
  gettimeofday (&end, 0);
  std::fprintf (stderr, "%.3fs\n", end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1e-6);
  std::fprintf (stderr, "# sentences: %ld\n", num_sent);
  std::fprintf (stderr, "acc. %.4f (corr %d) (incorr %d)\n",
                (seen_corr + unseen_corr) * 1.0 / (seen_corr + unseen_corr + seen_incorr + unseen_incorr),
                seen_corr + unseen_corr, seen_incorr + unseen_incorr);
  std::fprintf (stderr, "  (seen)   %.4f (corr %d) (incorr %d)\n",
                seen_corr * 1.0 / (seen_corr + seen_incorr), seen_corr, seen_incorr);
  std::fprintf (stderr, "  (unseen) %.4f (corr %d) (incorr %d)\n",
                unseen_corr * 1.0 / (unseen_corr + unseen_incorr), unseen_corr, unseen_incorr);
}