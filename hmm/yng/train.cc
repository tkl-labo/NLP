// train.cc -- learning parameters for first-order HMM
#include <sys/time.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <bnt.h>

struct stat_t {
  std::vector <std::string> id2tag, id2word;
  std::vector <int> init;
  std::vector <std::vector <int> > transition; // tag  -> tag
  std::vector <std::vector <int> > emission;   // word <- tag
  std::vector <int> count_word, count_tag;
  size_t num_sents;
  size_t num_tokens;
  stat_t () : id2tag (), id2word (), transition (), emission (), count_word (), count_tag (), num_sents (0), num_tokens (0) {}
  size_t num_tags  () const { return id2tag.size  (); }
  size_t num_words () const { return id2word.size (); }
  int add_word (const char* s, size_t len) { // new word
    id2word.push_back (std::string (s, len));
    count_word.push_back (0);
    emission.push_back (std::vector <int> (num_tags (), 0));
    return static_cast <int> (num_words ());
  }
  int add_tag (const char* s, size_t len) { // new tag
    init.push_back (0);
    for (size_t i = 0; i < transition.size (); ++i)
      transition[i].push_back (0);
    id2tag.push_back (std::string (s, len));
    count_tag.push_back (0);
    transition.push_back (std::vector <int> (num_tags (), 0));
    for (size_t i = 0; i < emission.size (); ++i)
      emission[i].push_back (0);
    return static_cast <int> (num_tags ());
  }
};

void write_model (const stat_t& stat, const int threshold) {
  // compute tag distribution for capitalized/uncapitalized unknown words
  const size_t V = stat.num_words ();
  const size_t N = stat.num_tags  ();
  std::vector <std::vector <int> > unk (NUM_UNK_TYPE, std::vector <int> (N, 0));
  std::vector <int> unk_all (N, 0);
  //
  for (size_t i = 0; i < V; ++i)
    if (stat.count_word[i] <= threshold) {
      unk_type ut = unk_classify (&stat.id2word[i][0],
                                  &stat.id2word[i][stat.id2word[i].size () - 1]);
      for (size_t j = 0; j < N; ++j) {
        unk[ut][j] += stat.emission[i][j];
        unk_all[j] += stat.emission[i][j];
      }
    }
  { // output tags
    for (size_t i = 0; i < N; ++i)
      std::fprintf (stdout, "%s\n", stat.id2tag[i].c_str ());
    std::fprintf (stdout, "\n");
  }
  { // output words
    for (size_t i = 0; i < V; ++i)
      std::fprintf (stdout, "%s\n", stat.id2word[i].c_str ());
    std::fprintf (stdout, "\n");
  }
  { // output init
    for (size_t i = 0; i < N; ++i)
      std::fprintf (stdout, "%g ", stat.init[i] * 1.0 / stat.num_sents);
    std::fprintf (stdout, "\n");
    std::fprintf (stdout, "\n");
  }
  { // output transition
    for (size_t i = 0; i < N; ++i) {
      int sum = 0;
      static const double lambda = 0.2;
      for (size_t j = 0; j < N; ++j) sum += stat.transition[i][j];
      for (size_t j = 0; j < N; ++j)
      // std::fprintf (stdout, "%g ", // naive laplace smoothing
      //               (transition[i][j] + 1) * 1.0 / (sum + V));
      std::fprintf (stdout, "%g ", // naive interpolation
                    lambda * stat.count_tag[j] / stat.num_tokens + (1 - lambda) * stat.transition[i][j] / sum);
      std::fprintf (stdout, "\n");
    }
    std::fprintf (stdout, "\n");
  }
  { // output tag distribution for unknown words
    for (int i = 0; i < NUM_UNK_TYPE; ++i) {
      for (size_t j = 0; j < N; ++j)
        std::fprintf (stdout, "%g ", unk[i][j] * 1.0 / (stat.count_tag[j] + unk_all[j]));
      std::fprintf (stdout, "\n");
    }
  }
  { // output emission
    for (size_t i = 0; i < V; ++i) {
      for (size_t j = 0; j < N; ++j)
        std::fprintf (stdout, "%g ", stat.emission[i][j] * 1.0 / (stat.count_tag[j] + unk_all[j]));
      std::fprintf (stdout, "\n");
    }
  }
}

int main (int argc, char** argv) {
  if (argc < 2) {
    std::fprintf (stderr, "Usage: %s threshold < train > model\n", argv[0]);
    std::exit (1);
  }
  int threshold = static_cast <int> (std::strtol (argv[1], NULL, 10));
  //
  struct timeval start, end;
  sbag_t tag2id, word2id;
  stat_t stat;
  //
  int word (-1), tag (-1), tag_prev (-1);
  char line[1024];
  gettimeofday (&start, 0);
  std::fprintf (stderr, "collecting statistics: ");
  while (std::fgets (line, 1024, stdin) != NULL) {
    if (line[0] == '\n'){
      tag_prev = -1;
      if (++stat.num_sents % 1000 == 0)
        std::fprintf (stderr, ".");
      continue;
    }
    char* p = line;
    { // word to id
      char* q = p; while (*p != ' ') ++p;
      int& id = word2id.update (q, p - q);
      if (! id) id = stat.add_word (q, p - q);
      word = id - 1;
      ++stat.num_tokens;
    }
    ++p;
    { // tag to id
      char* q = p; while (*p != ' ') ++p;
      int& id = tag2id.update (q, p - q);
      if (! id) id = stat.add_tag (q, p - q);
      tag = id - 1;
    }
    ++stat.count_tag[tag];
    ++stat.count_word[word];
    ++stat.emission[word][tag];
    if (tag_prev == -1)
      ++stat.init[tag];
    else
      ++stat.transition[tag_prev][tag];
    tag_prev = tag;
  }
  gettimeofday (&end, 0);
  std::fprintf (stderr, "%.3fs\n", end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1e-6);
  write_model (stat, threshold);
  //
  std::fprintf (stderr, "# sentences: %ld\n", stat.num_sents);
  std::fprintf (stderr, "# tokens: %ld\n", stat.num_tokens);
  std::fprintf (stderr, "# tags: %ld\n",  stat.num_tags  ());
  std::fprintf (stderr, "# words: %ld\n", stat.num_words ());
  return 0;
}
