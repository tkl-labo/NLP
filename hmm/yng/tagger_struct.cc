// tagger.cc -- hidden markov perceptron
#include <sys/time.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <limits>
#include <vector>
#include <algorithm>
#include <cedarpp.h>

typedef cedar::da <int> trie_t;

class sbag_t { // string bag
public:
  sbag_t () : str2id_ (), id2str_ () {}
  ~sbag_t ()
  { for (size_t i = 0; i < id2str_.size (); ++i) delete [] id2str_[i]; }
  int str2id (const char* p, const size_t len) const
  { return str2id_.exactMatchSearch <int> (p, len); }
  int add (const char* p, const size_t len) {
    char* str = new char[len + 1];
    str[len] = '\0';
    std::memcpy (str, p, len);
    id2str_.push_back (str);
    return str2id_.update (p, len) = id2str_.size () - 1;
  }
  const char*  id2str   (const int id) const { return id2str_[id]; }
  const size_t num_keys () const { return id2str_.size (); }
private:
  trie_t str2id_;
  std::vector <const char*> id2str_;
};

template <typename T>
struct cmp_t
{ bool operator() (const T& a, const T& b) const { return a.first > b.first; } };

size_t compute_features (const sbag_t& word_bag,
                         const trie_t& bigram_trie,
                         const trie_t& prefix_trie,
                         const trie_t& suffix_trie,
                         const std::vector <int>& words,
                         std::vector <std::vector <int> >& features,
                         const size_t bigram_size,
                         const size_t prefix_size,
                         const size_t suffix_size,
                         const size_t n) {
  size_t fi = 0;
  features.clear ();
  features.reserve (words.size ());
  static std::vector <int> feature;
  for (size_t i = 0; i < words.size (); ++i) {
    static size_t fi_ = 0;
    feature.clear ();
    fi = 0;
    if (i >= 2 && words[i - 2] < static_cast <int> (n))
      feature.push_back (fi + words[i - 2]);
    fi += n;
    if (i >= 1 && words[i - 1] < static_cast <int> (n))
      feature.push_back (fi + words[i - 1]);
    fi += n;
    if (words[i] < static_cast <int> (n))
      feature.push_back (fi + words[i]);
    fi += n;
    if (i + 1 < words.size () && words[i + 1] < static_cast <int> (n))
      feature.push_back (fi + words[i + 1]);
    fi += n;
    if (i >= 1) {
      unsigned char key_[16];
      size_t len_ = 0;
      int fn = words[i - 1] + 1;
      for (key_[len_] = (fn & 0x7f); fn >>= 7; key_[++len_] = (fn & 0x7f))
        key_[len_] |= 0x80;
      ++len_;
      fn = words[i] + 1;
      for (key_[len_] = (fn & 0x7f); fn >>= 7; key_[++len_] = (fn & 0x7f))
        key_[len_] |= 0x80;
      ++len_;
      int n = bigram_trie.exactMatchSearch <int> (reinterpret_cast <char*> (key_), len_);
      if (n >= 0) feature.push_back (fi + n);
    }
    fi += bigram_size;
    if (i + 1 < words.size ()) {
      unsigned char key_[16];
      size_t len_ = 0;
      int fn = words[i] + 1;
      for (key_[len_] = (fn & 0x7f); fn >>= 7; key_[++len_] = (fn & 0x7f))
        key_[len_] |= 0x80;
      ++len_;
      fn = words[i + 1] + 1;
      for (key_[len_] = (fn & 0x7f); fn >>= 7; key_[++len_] = (fn & 0x7f))
        key_[len_] |= 0x80;
      ++len_;
      int n = bigram_trie.exactMatchSearch <int> (reinterpret_cast <char*> (key_), len_);
      if (n >= 0) feature.push_back (fi + n);
    }
    fi += bigram_size;
    const char* word = word_bag.id2str (words[i]);
    bool has_hyphen = false;
    for (const char* p = word; *p && ! has_hyphen; ++p)
      has_hyphen |= *p == '-';
    if (has_hyphen) feature.push_back (fi); ++fi;
    bool has_capital = false;
    for (const char* p = word; *p && ! has_capital; ++p)
      has_capital |= *p >= 'A' && *p <= 'Z';
    if (has_capital) feature.push_back (fi); ++fi;
    bool has_number = false;
    for (const char* p = word; *p && ! has_number; ++p)
      has_number |= *p >= '0' && *p <= '9';
    if (has_number) feature.push_back (fi); ++fi;
    // prefix
    for (size_t from (0), pos (0); pos <= 4 && word[pos]; ) {
      int n = prefix_trie.traverse (word, from, pos, pos + 1);
      if (n < 0) break;
      feature.push_back (fi + n);
    }
    fi += prefix_size;
    const char* end = word + std::strlen (word);
    // suffix
    for (size_t from (0), pos (1); word + pos <= end; ++pos) {
      size_t pos_ = 0;
      int n = suffix_trie.traverse (end - pos, from, pos_ = 0, 1);
      if (n < 0) break;
      feature.push_back (fi + n);
    }
    fi += suffix_size;
    assert (! fi_ || fi == fi_);
    features.push_back (feature);
  }
  return fi;
}

double viterbi (const std::vector <int>& words, std::vector <int>& tags_,
                const std::vector <double*>& a,
                const std::vector <std::vector <double> >& b,
                const std::vector <double>& a_max,
                const size_t m) {
  size_t n = words.size ();
  // avoid memory reallocation for viterbi matrix
  static std::vector <std::vector <std::pair <double, size_t> > > score;
  static std::vector <std::vector <size_t> > bptr;
  if (score.size () < n) {
    for (size_t i = 0; i < score.size (); ++i) {
      score[i].resize (m);
      bptr[i].resize  (m, 0);
    }
    score.resize (n, std::vector <std::pair <double, size_t> > (m));
    bptr.resize (n, std::vector <size_t> (m, 0));
    tags_.resize (n);
  }
  // initialize
  for (size_t j = 0; j < m; ++j)
    score[0][j] = std::pair <double, size_t> (a[j][m] + b[0][j], j);
  for (size_t i = 1; i < n; ++i) { // for each word
    std::sort (score[i-1].begin (), score[i-1].end (),
               cmp_t <std::pair <double, size_t> > ());
    for (size_t j = 0; j < m; ++j) { // for each tag
      int max_k = 0;
      double max_score = - std::numeric_limits <double>::infinity ();
      for (size_t k = 0; k < m; ++k) { // for each previous tag
        if (max_score > score[i-1][k].first + a_max[j]) break;
        double val = score[i-1][k].first + a[j][score[i-1][k].second];
        if (max_score <= val)
          max_score = val, max_k = k;
      }
      score[i][j] = std::pair <double, size_t> (max_score + b[i][j], j);
      bptr[i][j]  = max_k;
    }
  }
  // finalize
  std::sort (score[n-1].begin (), score[n-1].end (),
             cmp_t <std::pair <double, size_t> > ());
  size_t max_k = 0;
  double max_score = - std::numeric_limits <double>::infinity ();
  for (size_t k = 0; k < m; ++k) {
    if (max_score > score[n-1][k].first + a_max[m]) break;
    const int tag = score[n-1][k].second;
    double val = score[n-1][k].first + a[m][tag];
    if (max_score <= val)
      max_score = val, max_k = k;
  }
  for (size_t k = max_k; n--; ) {
    const int tag = score[n][k].second;
    tags_[n] = tag;
    k = bptr[n][tag];
  }
  return max_score;
}

int main (int argc, char** argv) {
  if (argc < 5) {
    std::fprintf (stderr, "Usage: %s train model test niter [-q]\n", argv[0]);
    std::exit (1);
  }
  //
  struct timeval start, end;
  std::vector <std::vector <int> > wordss, tagss, features;
  std::vector <std::vector <std::vector <int> > > featuress;
  sbag_t word_bag, tag_bag;
  trie_t prefix_trie, suffix_trie, bigram_trie;
  size_t num_tokens (0), num_sents (0);
  size_t prefix_size (0), suffix_size (0), bigram_size (0);
  std::vector <int> words, tags, tags_;
  char line[1 << 21];
  { // read data
    std::fprintf (stderr, "read: ");
    gettimeofday (&start, 0);
    FILE* fp = std::fopen (argv[1], "r");
    while (std::fgets (line, 1 << 21, fp) != NULL) {
      if (line[0] == '\n') {
        num_tokens += words.size ();
        for (size_t i = 0; i < words.size () - 1; ++i) {
          unsigned char key_[16];
          size_t len_ = 0;
          int fn = words[i] + 1;
          for (key_[len_] = (fn & 0x7f); fn >>= 7; key_[++len_] = (fn & 0x7f))
            key_[len_] |= 0x80;
          ++len_;
          fn = words[i + 1] + 1;
          for (key_[len_] = (fn & 0x7f); fn >>= 7; key_[++len_] = (fn & 0x7f))
            key_[len_] |= 0x80;
          ++len_;
          int n = bigram_trie.exactMatchSearch <int> (reinterpret_cast <char*> (key_), len_);
          if (n == -1)
            n = bigram_trie.update (reinterpret_cast <char*> (key_), len_) = bigram_size++;
        }
        wordss.push_back (words);
        tagss.push_back (tags);
        if (++num_sents % 1000 == 0) std::fprintf (stderr, ".");
        words.clear ();
        tags.clear ();
      } else {
        char* p = line;
        char* word = p;   while (*p != ' ') ++p;
        int id = word_bag.str2id (word, p - word);
        words.push_back (id != -1 ? id : word_bag.add (word, p - word));
        if (id == -1) { // register suffix and prefix
          bool flag = true;
          for (size_t from (0), pos (0); word + pos != p && pos < 6; ) {
            if (flag)
              if (prefix_trie.traverse (word, from, pos, pos + 1) >= 0) continue;
            prefix_trie.update (word, from, pos, pos + 1)  = prefix_size++;
            flag = false;
          }
          flag = true;
          for (size_t from (0), pos (1); word + pos <= p && pos <= 6; ++pos) {
            size_t pos_ = 0;
            if (flag)
              if (suffix_trie.traverse (p - pos, from, pos_, 1) >= 0) continue;
            suffix_trie.update (p - pos, from, pos_ = 0, 1) = suffix_size++;
            flag = false;
          }
        }
        char* tag  = ++p; while (*p != ' ') ++p;
        id = tag_bag.str2id (tag, p - tag);
        tags.push_back (id != -1 ? id : tag_bag.add (tag, p - tag));
      }
    }
    std::fclose (fp);
    gettimeofday (&end, 0);
    std::fprintf (stderr, "%.3fs\n", end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1e-6);
    std::fprintf (stderr, "  # sentences: %ld\n", num_sents);
    std::fprintf (stderr, "  # tokens: %ld\n",    num_tokens);
    std::fprintf (stderr, "  # words: %ld\n",     word_bag.num_keys ());
    std::fprintf (stderr, "  # tags: %ld\n",      tag_bag.num_keys ());
  }
  const size_t m = tag_bag.num_keys  (); // m - 1 for BOS/EOS
  const size_t n = word_bag.num_keys ();
  featuress.resize (num_sents);
  size_t fi = 0;
  { // compute features
    std::fprintf (stderr, "setup: ");
    gettimeofday (&start, 0);
    for (size_t i = 0; i < num_sents; ++i) {
      fi = compute_features (word_bag, bigram_trie, prefix_trie, suffix_trie, wordss[i], featuress[i], bigram_size, prefix_size, suffix_size, n);
    }
    gettimeofday (&end, 0);
    std::fprintf (stderr, "%.3fs\n", end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1e-6);
  }
  const size_t num_features = (m + 1) * (m + 1) + fi * m;
  double* w = new double[num_features]; std::fill_n (w, num_features, 0.0);
  double* v = new double[num_features]; std::fill_n (v, num_features, 0.0);
  std::vector <double*> a (m + 1), a_ (m + 1);
  std::vector <std::vector <double> > b;
  std::vector <double>  a_max (m + 1, std::numeric_limits <double>::infinity ());
  for (size_t i = 0; i < m + 1; ++i)
    a[i] = &w[(m + 1) * i], a_[i] = &v[(m + 1) * i];
  { // train
    std::fprintf (stderr, "train: ");
    gettimeofday (&start, 0);
    size_t nex = 0;
    std::vector <size_t> nex_ (num_features, 0);
    std::vector <int> f;
    size_t iter = std::strtol (argv[4], NULL, 10);
    for (size_t i = 0; i < iter; ++i) {
      for (size_t j = 0; j < num_sents; ++j) {
        ++nex;
        const std::vector <int>& words = wordss[j];
        const std::vector <int>& tags  = tagss[j];
        const std::vector <std::vector <int> >& features = featuress[j];
        if (b.size () < words.size ())
          b.resize (words.size (), std::vector <double> (m, 0.0));
        f.clear ();
        for (size_t k = 0; k < words.size (); ++k) {
          std::fill (b[k].begin (), b[k].end (), 0.0);
          for (size_t l = 0; l < features[k].size (); ++l) {
            f.push_back (features[k][l]);
            for (size_t p = 0; p < m; ++p)
              b[k][p]  += w[(m + 1) * (m + 1) + features[k][l] * m + p];
          }
        }
        std::sort (f.begin (), f.end ());
        f.erase (std::unique (f.begin (), f.end ()), f.end ());
        for (size_t k = 0; k < f.size (); ++k)
          if (! nex_[f[k]]) nex_[f[k]] = nex;
        int pre (m), pre_ (m);
        viterbi (words, tags_, a, b, a_max, m);
        pre_ = m;
        for (size_t k = 0; k < words.size (); pre = tags[k], pre_ = tags_[k], ++k) {
          if (tags[k] != tags_[k])
            for (size_t l = 0; l < features[k].size (); ++l) {
              const size_t offset = (m + 1) * (m + 1) + features[k][l] * m;
              double* w_ = &w[offset];
              double* v_ = &v[offset];
              w_[tags[k]]  += 1; v_[tags[k]]  += nex;
              w_[tags_[k]] -= 1; v_[tags_[k]] -= nex;
            }
          if (tags[k] != tags_[k] || pre != pre_) {
            a[tags[k]][pre]   += 1; a_[tags[k]][pre]   += nex;
            a[tags_[k]][pre_] -= 1; a_[tags_[k]][pre_] -= nex;
          }
        }
        if (pre != pre_) {
          a[m][pre]  += 1; a_[m][pre]  += nex;
          a[m][pre_] -= 1; a_[m][pre_] -= nex;
        }
        for (size_t k = 0; k <= m; ++k)
          a_max[k] = *std::max_element (a[k], a[k] + m);
      }
      std::fprintf (stderr, ".");
    }
    // averaging
    for (size_t i = 0; i < num_features; ++i) w[i] -= v[i] / (nex + 1);
    for (size_t i = 0; i < fi; ++i) {
      if (nex_[i]) {
        // std::fprintf (stderr, "%ld %ld\n", i, nex_[i]);
        for (size_t j = 0; j < m; ++j)
          w[(m + 1) * (m + 1) + i * m + j] *= (nex + 1) / ((nex - nex_[i] + 1.0) + 1);
      }
    }
    for (size_t k = 0; k <= m; ++k)
      a_max[k] = *std::max_element (a[k], a[k] + m);
    gettimeofday (&end, 0);
    std::fprintf (stderr, "%.3fs\n", end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1e-6);
  }
  { // test
    bool quiet = argc > 5 && std::strcmp (argv[5], "-q") == 0;
    num_tokens = num_sents = 0;
    std::fprintf (stderr, "test: ");
    gettimeofday (&start, 0);
    FILE* fp = std::fopen (argv[3], "r");
    int sc (0), si (0), uc (0), ui (0);
    while (std::fgets (line, 1 << 21, fp) != NULL) {
      if (line[0] == '\n') {
        num_tokens += words.size ();
        compute_features (word_bag, bigram_trie, prefix_trie, suffix_trie, words, features, bigram_size, prefix_size, suffix_size, n);
        if (b.size () < words.size ())
          b.resize (words.size (), std::vector <double> (m, 0.0));
        for (size_t k = 0; k < words.size (); ++k) {
          std::fill (b[k].begin (), b[k].end (), 0.0);
          for (size_t l = 0; l < features[k].size (); ++l)
            for (size_t p = 0; p < m; ++p)
              b[k][p]  += w[(m + 1) * (m + 1) +  features[k][l] * m + p];
        }
        viterbi (words, tags_, a, b, a_max, m);
        for (size_t i = 0; i < tags.size (); ++i) {
          if (tags[i] == tags_[i])
            { if (words[i] < static_cast <int> (n)) ++sc; else ++uc; }
          else
            { if (words[i] < static_cast <int> (n)) ++si; else ++ui; }
        }
#ifndef NDEBUG
        bool success = true;
        char str[1 << 21];
        char* p = str;
        for (size_t i = 0; i < words.size (); ++i) {
          success &= tags[i] == tags_[i];
          if (tags[i] != tags_[i]) p += std::sprintf (p, "\x1b[31m");
          if (words[i] >= static_cast <int> (n)) p += std::sprintf (p, "*");
          p += std::sprintf (p, "%s/%s/%s ",
                             word_bag.id2str (words[i]),
                             tag_bag.id2str (tags[i]),
                             tag_bag.id2str (tags_[i]));
          if (tags[i] != tags_[i]) p += std::sprintf (p, "\x1b[39m");
        }
        if (! quiet || ! success)
          std::fprintf (stdout, "%s\n", str);
#endif
        if (++num_sents % 1000 == 0) std::fprintf (stderr, ".");
        words.clear ();
        tags.clear ();
      } else {
        char* p = line;
        char* word = p;   while (*p != ' ') ++p;
        int id = word_bag.str2id (word, p - word);
        words.push_back (id != -1 ? id : word_bag.add (word, p - word));
        char* tag  = ++p; while (*p != ' ') ++p;
        id = tag_bag.str2id (tag, p - tag);
        if (id == -1) id = tag_bag.add (tag, p - tag);
        tags.push_back (id);
      }
    }
    std::fclose (fp);
    //
    gettimeofday (&end, 0);
    std::fprintf (stderr, "%.3fs\n", end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1e-6);
    std::fprintf (stderr, "  # sentences: %ld\n", num_sents);
    std::fprintf (stderr, "  # tokens: %ld\n",    num_tokens);
    std::fprintf (stderr, "acc. %.4f (corr %d) (incorr %d)\n",
                  (sc + uc) * 1.0 / num_tokens, sc + uc, si + ui);
    std::fprintf (stderr, "  (seen)   %.4f (corr %d) (incorr %d)\n",
                  sc * 1.0 / (sc + si), sc, si);
    std::fprintf (stderr, "  (unseen) %.4f (corr %d) (incorr %d)\n",
                  uc * 1.0 / (uc + ui), uc, ui);
    //
    delete [] w;
    delete [] v;
  }
}
