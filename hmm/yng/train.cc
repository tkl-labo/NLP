// train.cc -- learning parameters for first-order HMM
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <cedarpp.h>

typedef cedar::da <int>  sbag_t;

int main (int argc, char** argv) {
  if (argc < 2) {
    std::fprintf (stderr, "Usage: %s threshold < train > model\n", argv[0]);
    std::exit (1);
  }
  int threshold = static_cast <int> (std::strtol (argv[1], NULL, 10));
  //
  sbag_t tag2id;
  sbag_t word2id;
  std::vector <std::string> id2tag, id2word;
  std::vector <int> init;
  std::vector <std::vector <int> > transition; // tag  -> tag
  std::vector <std::vector <int> > emission;   // word <- tag (transposed)
  std::vector <int> count_word, count_tag;
  //
  int word (-1), tag (-1), tag_prev (-1);
  size_t num_sent = 0;
  char line[1024];
  while (std::fgets (line, 1024, stdin) != NULL) {
    if (line[0] == '\n'){
      tag_prev = -1;
      ++num_sent;
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
  // compute tag distribution for unknown words
  std::vector <int> unk (id2tag.size (), 0);
  for (size_t i = 0; i < emission.size (); ++i)
    if (count_word[i] <= threshold)
      for (size_t j = 0; j < emission[i].size (); ++j)
        unk[j] += emission[i][j];
  //
  // output tags
  for (size_t i = 0; i < id2tag.size (); ++i)
    std::fprintf (stdout, "%s\n", id2tag[i].c_str ());
  std::fprintf (stdout, "\n");
  // output words
  size_t num_words_as_unknown = 0;
  for (size_t i = 0; i < id2word.size (); ++i)
    if (count_word[i] > threshold)
      std::fprintf (stdout, "%s\n", id2word[i].c_str ());
    else
      ++num_words_as_unknown;
  std::fprintf (stdout, "\n");
  // output init
  for (size_t i = 0; i < init.size (); ++i)
    std::fprintf (stdout, "%g ", init[i] * 1.0 / num_sent);
  std::fprintf (stdout, "\n");
  std::fprintf (stdout, "\n");
  // output transition
  for (size_t i = 0; i < transition.size (); ++i) {
    int sum = 0;
    for (size_t j = 0; j < transition[i].size (); ++j) sum += transition[i][j];
    for (size_t j = 0; j < transition[i].size (); ++j)
      std::fprintf (stdout, "%g ", // naive laplace smoothing
                    (transition[i][j] + 1) * 1.0 / (sum + transition.size ()));
    std::fprintf (stdout, "\n");
  }
  std::fprintf (stdout, "\n");
  // output tag distribution for unknown words
  for (size_t i = 0; i < unk.size (); ++i)
    std::fprintf (stdout, "%g ", unk[i] * 1.0 / count_tag[i]);
  std::fprintf (stdout, "\n");
  // output emission
  for (size_t i = 0; i < emission.size (); ++i)
    if (count_word[i] > threshold) {
      for (size_t j = 0; j < emission[i].size (); ++j)
        std::fprintf (stdout, "%g ", emission[i][j] * 1.0 / count_tag[j]);
      std::fprintf (stdout, "\n");
    }
  //
  std::fprintf (stderr, "# sentences: %ld\n", num_sent);
  std::fprintf (stderr, "# tags: %ld\n", id2tag.size ());
  std::fprintf (stderr, "# words: %ld\n", id2word.size ());
  std::fprintf (stderr, "# words (above threshold): %ld\n", id2word.size () - num_words_as_unknown);
  return 0;
}
