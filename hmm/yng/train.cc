// train.cc -- learning parameters for first-order HMM
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <cedar.h>

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
  std::vector <std::string> id2tag;
  std::vector <std::string> id2word;
  std::vector <int> init;
  std::vector <std::vector <int> > transition; // tag  -> tag
  std::vector <std::vector <int> > emission;   // word -> tag
  std::vector <int> count_word;
  std::vector <int> count_tag;
  //
  int num_sent = 0;
  int word = -1;
  int tag  = -1;
  int tag_prev = -1;
  char line[1024];
  while (std::fgets (line, 1024, stdin) != NULL) {
    if (line[0] == '\n') {
      tag_prev = -1;
      ++num_sent;
    } else {
      char* p = line;
      {
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
      {
        char* q = p; while (*p != ' ') ++p;
        int& id = tag2id.update (q, p - q);
        if (! id) {
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
  }
  // compute tag distribution for unknown words
  std::vector <int> unk (id2tag.size (), 0);
  for (size_t i = 0; i < emission.size (); ++i)
    if (count_word[i] <= threshold)
      for (size_t j = 0; j < emission[i].size (); ++j)
        unk[j] += emission[i][j];
  //
  // output tags
  std::fprintf (stderr, "# tags: %ld\n", id2tag.size ()); 
  for (std::vector <std::string>::iterator it = id2tag.begin ();
       it != id2tag.end (); ++it)
    std::fprintf (stdout, "%s\n", it->c_str ());
  std::fprintf (stdout, "\n");
  // output words
  std::fprintf (stderr, "# words: %ld\n", id2word.size ());
  size_t num_words_below_threshold = 0;
  for (size_t i = 0; i < id2word.size (); ++i)
    if (count_word[i] > threshold)
      std::fprintf (stdout, "%s\n", id2word[i].c_str ());
    else
      ++num_words_below_threshold;
  std::fprintf (stdout, "\n");
  std::fprintf (stderr, "# words (above threshold): %ld\n", id2word.size () - num_words_below_threshold);
  // output init
  for (std::vector <int>::iterator it = init.begin (); it != init.end (); ++it)
    std::fprintf (stdout, "%g ", *it * 1.0 / num_sent);
  std::fprintf (stdout, "\n");
  std::fprintf (stdout, "\n");
  // output transition
  for (size_t i = 0; i < transition.size (); ++i) {
    int sum = 0;
    for (size_t j = 0; j < transition[i].size (); ++j) sum += transition[i][j];
    for (size_t j = 0; j < transition[i].size (); ++j)
      std::fprintf (stdout, "%g ", // naive laplace smoothing
                    (transition[i][j] + 1) * 1.0 / (sum + id2tag.size ()));
    std::fprintf (stdout, "\n");
  }
  std::fprintf (stdout, "\n");
  // output tag distribution for unknown word
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
  return 0;
}
