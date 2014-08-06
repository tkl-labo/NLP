#include <cstring>
#include <cedarpp.h>

typedef cedar::da <int>  sbag_t;

enum unk_type { HYPHEN, CAP, CD, CDE, END_S, END_ED, END_EN, END_ING, END_ER, END_EST, END_NOUN, END_VERB, END_ADV, END_ADJ, OTHER, NUM_UNK_TYPE };

unk_type unk_classify (const char* beg, const char* end) {
  for (const char* p = beg; p <= end; ++p) if (*p == '-') return HYPHEN;
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

