// train.cc -- learning parameters for first-order HMM
#include <sys/time.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
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

int main (int argc, char** argv) {
  if (argc < 2) {
    std::fprintf (stderr, "Usage: %s threshold < train > model\n", argv[0]);
    std::exit (1);
  }
  int threshold = static_cast <int> (std::strtol (argv[1], NULL, 10));
  //
  struct timeval start, end;
  sbag_t tag2id;
  sbag_t word2id;
  std::vector <std::string> id2tag, id2word;
  std::vector <int> init;
  std::vector <std::vector <int> > transition; // tag  -> tag
  std::vector <std::vector <int> > emission;   // word <- tag (transposed)
  std::vector <int> count_word, count_tag;
  //
  int word (-1), tag (-1), tag_prev (-1);
  size_t num_sent (0), num_tokens (0);
  char line[1024];
  gettimeofday (&start, 0);
  std::fprintf (stderr, "collecting statistics: ");
  while (std::fgets (line, 1024, stdin) != NULL) {
    if (line[0] == '\n'){
      tag_prev = -1;
      if (++num_sent % 1000 == 0)
        std::fprintf (stderr, ".");
      continue;
    }
    char* p = line;
    { // word to id
      char* q = p; while (*p != ' ') ++p;
      int& id = word2id.update (q, p - q);
      if (! id) { // new word
        id2word.push_back (std::string (q, p - q));
        count_word.push_back (0);
        emission.push_back (std::vector <int> (id2tag.size (), 0));
        id = static_cast <int> (id2word.size ());
      }
      word = id - 1;
      ++num_tokens;
    }
    ++p;
    { // tag to id
      char* q = p; while (*p != ' ') ++p;
      int& id = tag2id.update (q, p - q);
      if (! id) { // new tag
        init.push_back (0);
        for (size_t i = 0; i < transition.size (); ++i)
          transition[i].push_back (0);
        id2tag.push_back (std::string (q, p - q));
        count_tag.push_back (0);
        id = static_cast <int> (id2tag.size ());
        transition.push_back (std::vector <int> (id2tag.size (), 0));
        for (size_t i = 0; i < emission.size (); ++i)
          emission[i].push_back (0);
      }
      tag = id - 1;
    }
    ++count_tag[tag];
    ++count_word[word];
    ++emission[word][tag];
    if (tag_prev == -1)
      ++init[tag];
    else
      ++transition[tag_prev][tag];
    tag_prev = tag;
  }
  gettimeofday (&end, 0);
  std::fprintf (stderr, "%.3fs\n", end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1e-6);
  // compute tag distribution for capitalized/uncapitalized unknown words
  // -ful -ed -tive -ous -ry
  std::vector <std::vector <int> > unk (NUM_UNK_TYPE, std::vector <int> (id2tag.size (), 0));
  std::vector <int> unk_all (id2tag.size (), 0);
  for (size_t i = 0; i < emission.size (); ++i)
    if (count_word[i] <= threshold) {
      unk_type ut = unk_classify (&id2word[i][0], &id2word[i][id2word[i].size () - 1]);
      for (size_t j = 0; j < emission[i].size (); ++j) {
        unk[ut][j] += emission[i][j];
        unk_all[j] += emission[i][j];
      }
    }
  //
  // output tags
  for (size_t i = 0; i < id2tag.size (); ++i)
    std::fprintf (stdout, "%s\n", id2tag[i].c_str ());
  std::fprintf (stdout, "\n");
  // output words
  size_t num_words_as_unknown = 0;
  for (size_t i = 0; i < id2word.size (); ++i) {
    std::fprintf (stdout, "%s\n", id2word[i].c_str ());
    if (count_word[i] <= threshold)
      ++num_words_as_unknown;
  }
  std::fprintf (stdout, "\n");
  // output init
  for (size_t i = 0; i < init.size (); ++i)
    std::fprintf (stdout, "%g ", init[i] * 1.0 / num_sent);
  std::fprintf (stdout, "\n");
  std::fprintf (stdout, "\n");
  // output transition
  for (size_t i = 0; i < transition.size (); ++i) {
    int sum = 0;
    static const double lambda = 0.2;
    for (size_t j = 0; j < transition[i].size (); ++j) sum += transition[i][j];
    for (size_t j = 0; j < transition[i].size (); ++j)
      // std::fprintf (stdout, "%g ", // naive laplace smoothing
      //               (transition[i][j] + 1) * 1.0 / (sum + transition.size ()));
      std::fprintf (stdout, "%g ", // naive interpolation
                    lambda * count_tag[j] / num_tokens + (1 - lambda) * transition[i][j] / sum);
    std::fprintf (stdout, "\n");
  }
  std::fprintf (stdout, "\n");
  // output tag distribution for unknown words
  for (int i = 0; i < NUM_UNK_TYPE; ++i) {
    for (size_t j = 0; j < unk[i].size (); ++j)
      std::fprintf (stdout, "%g ", unk[i][j] * 1.0 / (count_tag[j] + unk_all[j]));
    std::fprintf (stdout, "\n");
  }
  // output emission
  for (size_t i = 0; i < emission.size (); ++i) {
    for (size_t j = 0; j < emission[i].size (); ++j)
      std::fprintf (stdout, "%g ", emission[i][j] * 1.0 / (count_tag[j] + unk_all[j]));
      std::fprintf (stdout, "\n");
  }
  //
  std::fprintf (stderr, "# sentences: %ld\n", num_sent);
  std::fprintf (stderr, "# tags: %ld\n",  id2tag.size ());
  std::fprintf (stderr, "# words: %ld\n", id2word.size ());
  std::fprintf (stderr, "# words (above threshold): %ld\n", id2word.size () - num_words_as_unknown);
  return 0;
}
