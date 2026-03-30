#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <gmp.h>

typedef struct Node {
  mpz_t val;
  struct Node* next;
} Node;

typedef struct {
  Node** buckets;
  size_t nbuckets;
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

static void mpzset_init(MPZSet* s, size_t nbuckets) {
  s->nbuckets = nbuckets;
  s->size = 0;
  s->buckets = (Node**)calloc(nbuckets, sizeof(Node*));
}

static void mpzset_clear(MPZSet* s) {
  if (!s->buckets) return;
  for (size_t i = 0; i < s->nbuckets; i++) {
    Node* cur = s->buckets[i];
    while (cur) {
      Node* nxt = cur->next;
      mpz_clear(cur->val);
      free(cur);
      cur = nxt;
    }
  }
  free(s->buckets);
  s->buckets = NULL;
  s->nbuckets = 0;
  s->size = 0;
}

static int mpzset_contains(MPZSet* s, const mpz_t x, uint64_t h) {
  size_t idx = (size_t)(h % s->nbuckets);
  Node* cur = s->buckets[idx];
  while (cur) {
    if (mpz_cmp(cur->val, x) == 0) return 1;
    cur = cur->next;
  }
  return 0;
}

static void mpzset_rehash(MPZSet* s, size_t new_buckets) {
  Node** old = s->buckets;
  size_t oldn = s->nbuckets;
  s->buckets = (Node**)calloc(new_buckets, sizeof(Node*));
  s->nbuckets = new_buckets;
  for (size_t i = 0; i < oldn; i++) {
    Node* cur = old[i];
    while (cur) {
      Node* nxt = cur->next;
      uint64_t h = hash_mpz(cur->val);
      size_t idx = (size_t)(h % s->nbuckets);
      cur->next = s->buckets[idx];
      s->buckets[idx] = cur;
      cur = nxt;
    }
  }
  free(old);
}

static void mpzset_add(MPZSet* s, const mpz_t x) {
  if (!s->buckets) mpzset_init(s, 1024);
  uint64_t h = hash_mpz(x);
  if (mpzset_contains(s, x, h)) return;
  Node* node = (Node*)malloc(sizeof(Node));
  mpz_init_set(node->val, x);
  size_t idx = (size_t)(h % s->nbuckets);
  node->next = s->buckets[idx];
  s->buckets[idx] = node;
  s->size++;
  if (s->size * 10 > s->nbuckets * 9) { // load factor > 0.9
    mpzset_rehash(s, s->nbuckets * 2);
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
        if (!dp[prev_s].buckets || dp[prev_s].size == 0) continue;
        unsigned long long factor = base * row[prev_s - 1];
        mpz_t tmp;
        mpz_init(tmp);
        for (size_t bi = 0; bi < dp[prev_s].nbuckets; bi++) {
          Node* cur = dp[prev_s].buckets[bi];
          while (cur) {
            if (factor == 1) {
              mpzset_add(&new_dp[s], cur->val);
            } else if (factor <= 0xFFFFFFFFULL) {
              mpz_mul_ui(tmp, cur->val, (unsigned long)factor);
              mpzset_add(&new_dp[s], tmp);
            } else {
              mpz_mul_ui(tmp, cur->val, (unsigned long)factor);
              mpzset_add(&new_dp[s], tmp);
            }
            cur = cur->next;
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
  if (dp[n].buckets) {
    for (size_t bi = 0; bi < dp[n].nbuckets; bi++) {
      Node* cur = dp[n].buckets[bi];
      while (cur) {
        mpz_divexact(q, cur->val, denom);
        mpzset_add(&coeffs, q);
        cur = cur->next;
      }
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
