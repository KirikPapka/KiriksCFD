#pragma once

// Shared tinyexpr helpers for exam cases (variables must point to stable member storage).

#include "tinyexpr.h"

#include <cstdint>
#include <cstring>

namespace teutil {

inline void freeExpr(void*& p) {
  if (p != nullptr) {
    te_free(static_cast<te_expr*>(p));
    p = nullptr;
  }
}

inline bool compileOne(const char* src, te_variable* vars, int nvar, void** out, int* errColOut) {
  int err = 0;
  te_expr* ex = te_compile(src, vars, nvar, &err);
  if (ex == nullptr) {
    *out = nullptr;
    *errColOut = err;
    return false;
  }
  *out = ex;
  *errColOut = 0;
  return true;
}

inline uint64_t fnv1a64(const char* s) {
  uint64_t h = 1469598103934665603ull;
  for (const char* p = s; *p != '\0'; ++p) {
    h ^= static_cast<uint64_t>(static_cast<unsigned char>(*p));
    h *= 1099511628211ull;
  }
  return h;
}

inline uint64_t hashPair(const char* a, const char* b) {
  uint64_t h = fnv1a64(a);
  h ^= 0x9e3779b97f4a7c15ull;
  h *= 1099511628211ull;
  h ^= fnv1a64(b);
  return h;
}

inline uint64_t hashTriplet(const char* a, const char* b, const char* c) {
  uint64_t h = fnv1a64(a);
  h ^= 0xFFull;
  h *= 1099511628211ull;
  h ^= fnv1a64(b);
  h ^= 0xFFull;
  h *= 1099511628211ull;
  h ^= fnv1a64(c);
  return h;
}

}  // namespace teutil
