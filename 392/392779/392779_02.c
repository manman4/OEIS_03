#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <gmp.h>

typedef struct {
  mpz_t* vals;
  unsigned char* used; // 0: empty, 1: filled
  size_t nslots;
  size_t size;
} MPZSet;

static uint64_t hash_mpz(const mpz_t x) {
  uint64_t h = 1469598103934665603ULL; // FNV-1a
  size_t nlimbs = mpz_size(x);
  if (nlimbs == 0) return h;
  if (nlimbs == 1) {
    mp_limb_t limb = mpz_getlimbn(x, 0);
    for (size_t b = 0; b < sizeof(mp_limb_t); b++) {
      h ^= (uint64_t)((limb >> (8 * b)) & 0xFF);
      h *= 1099511628211ULL;
    }
    return h;
  }
  for (size_t i = 0; i < nlimbs; i++) {
    mp_limb_t limb = mpz_getlimbn(x, i);
    for (size_t b = 0; b < sizeof(mp_limb_t); b++) {
      h ^= (uint64_t)((limb >> (8 * b)) & 0xFF);
      h *= 1099511628211ULL;
    }
  }
  return h;
}

static void mpzset_init(MPZSet* s, size_t nslots) {
  s->nslots = nslots;
  s->size = 0;
  s->vals = (mpz_t*)malloc(nslots * sizeof(mpz_t));
  s->used = (unsigned char*)calloc(nslots, sizeof(unsigned char));
}

static void mpzset_clear(MPZSet* s) {
  if (!s->used) return;
  for (size_t i = 0; i < s->nslots; i++) {
    if (s->used[i]) mpz_clear(s->vals[i]);
  }
  free(s->vals);
  free(s->used);
  s->vals = NULL;
  s->used = NULL;
  s->nslots = 0;
  s->size = 0;
}

static int mpzset_contains(MPZSet* s, const mpz_t x, uint64_t h) {
  size_t idx = (size_t)(h % s->nslots);
  for (size_t i = 0; i < s->nslots; i++) {
    size_t pos = (idx + i) % s->nslots;
    if (!s->used[pos]) return 0;
    if (mpz_cmp(s->vals[pos], x) == 0) return 1;
  }
  return 0;
}

static void mpzset_rehash(MPZSet* s, size_t new_slots) {
  mpz_t* old_vals = s->vals;
  unsigned char* old_used = s->used;
  size_t oldn = s->nslots;

  s->vals = (mpz_t*)malloc(new_slots * sizeof(mpz_t));
  s->used = (unsigned char*)calloc(new_slots, sizeof(unsigned char));
  s->nslots = new_slots;
  s->size = 0;

  for (size_t i = 0; i < oldn; i++) {
    if (!old_used[i]) continue;
    uint64_t h = hash_mpz(old_vals[i]);
    size_t idx = (size_t)(h % s->nslots);
    for (size_t j = 0; j < s->nslots; j++) {
      size_t pos = (idx + j) % s->nslots;
      if (!s->used[pos]) {
        mpz_init_set(s->vals[pos], old_vals[i]);
        s->used[pos] = 1;
        s->size++;
        break;
      }
    }
    mpz_clear(old_vals[i]);
  }
  free(old_vals);
  free(old_used);
}

static void mpzset_add(MPZSet* s, const mpz_t x) {
  if (!s->used) mpzset_init(s, 1024);
  uint64_t h = hash_mpz(x);
  if (mpzset_contains(s, x, h)) return;
  size_t idx = (size_t)(h % s->nslots);
  for (size_t i = 0; i < s->nslots; i++) {
    size_t pos = (idx + i) % s->nslots;
    if (!s->used[pos]) {
      mpz_init_set(s->vals[pos], x);
      s->used[pos] = 1;
      s->size++;
      break;
    }
  }
  if (s->size * 10 > s->nslots * 8) { // load factor > 0.8
    mpzset_rehash(s, s->nslots * 2);
  }
}

static unsigned long long** comb_table(int n) {
  unsigned long long** comb = (unsigned long long**)malloc((n + 1) * sizeof(unsigned long long*));
  for (int i = 0; i <= n; i++) {
    comb[i] = (unsigned long long*)calloc(n + 1, sizeof(unsigned long long));
    comb[i][0] = 1;
    comb[i][i] = 1;
    for (int j = 1; j < i; j++) {
      comb[i][j] = comb[i - 1][j - 1] + comb[i - 1][j];
    }
  }
  return comb;
}

static void free_comb(unsigned long long** comb, int n) {
  for (int i = 0; i <= n; i++) free(comb[i]);
  free(comb);
}

static void factorial_mpz(mpz_t out, int n) {
  mpz_set_ui(out, 1);
  for (int i = 2; i <= n; i++) mpz_mul_ui(out, out, (unsigned long)i);
}

static size_t distinct_coeff_count(int n, unsigned long long** comb, size_t max_set_size) {
  if (n == 0) return 1;

  MPZSet* dp = (MPZSet*)calloc(n + 1, sizeof(MPZSet));
  for (int s = 1; s <= n; s++) {
    mpz_t one;
    mpz_init_set_ui(one, 1);
    mpzset_add(&dp[s], one);
    mpz_clear(one);
  }

  for (int k = 2; k <= n; k++) {
    MPZSet* new_dp = (MPZSet*)calloc(n + 1, sizeof(MPZSet));
    for (int s = k; s <= n; s++) {
      unsigned long long base = (unsigned long long)(s - k + 1);
      unsigned long long* row = comb[s - 1];
      for (int prev_s = k - 1; prev_s <= s; prev_s++) {
        if (!dp[prev_s].used || dp[prev_s].size == 0) continue;
        unsigned long long factor = base * row[prev_s - 1];
        mpz_t tmp;
        mpz_init(tmp);
        for (size_t bi = 0; bi < dp[prev_s].nslots; bi++) {
          if (!dp[prev_s].used[bi]) continue;
          if (factor == 1) {
            mpzset_add(&new_dp[s], dp[prev_s].vals[bi]);
          } else {
            mpz_mul_ui(tmp, dp[prev_s].vals[bi], (unsigned long)factor);
            mpzset_add(&new_dp[s], tmp);
          }
        }
        mpz_clear(tmp);
        if (max_set_size > 0 && new_dp[s].size > max_set_size) {
          for (int ss = 0; ss <= n; ss++) mpzset_clear(&dp[ss]);
          free(dp);
          for (int ss = 0; ss <= n; ss++) mpzset_clear(&new_dp[ss]);
          free(new_dp);
          return 0;
        }
      }
    }
    for (int s = 0; s <= n; s++) mpzset_clear(&dp[s]);
    free(dp);
    dp = new_dp;
  }

  mpz_t denom;
  mpz_init(denom);
  factorial_mpz(denom, n - 1);

  MPZSet coeffs = {0};
  mpz_t q;
  mpz_init(q);
  if (dp[n].used) {
    for (size_t bi = 0; bi < dp[n].nslots; bi++) {
      if (!dp[n].used[bi]) continue;
      mpz_divexact(q, dp[n].vals[bi], denom);
      mpzset_add(&coeffs, q);
    }
  }
  size_t result = coeffs.size;
  mpz_clear(q);
  mpz_clear(denom);
  for (int s = 0; s <= n; s++) mpzset_clear(&dp[s]);
  free(dp);
  mpzset_clear(&coeffs);
  return result;
}

int main(int argc, char** argv) {
  int limit = 10;
  size_t max_set_size = 0;
  if (argc >= 2) limit = atoi(argv[1]);
  if (argc >= 3) max_set_size = (size_t)strtoull(argv[2], NULL, 10);
  if (limit < 0) limit = 0;

  unsigned long long** comb = comb_table(limit);
  for (int n = 0; n <= limit; n++) {
    size_t a_n = distinct_coeff_count(n, comb, max_set_size);
    if (a_n == 0) {
      fprintf(stderr, "stopped at n=%d (set size exceeded)\n", n);
      break;
    }
    printf("%d %zu\n", n, a_n);
  }
  free_comb(comb, limit);
  return 0;
}
