#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int n;
  mpz_t *bit;
} Fenwick;

typedef struct {
  int len;
  int *v;
} IntVec;

typedef struct {
  int len;
  mpz_t *a;
} BigRow;

static void die(const char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

static void *xmalloc(size_t n) {
  void *p = malloc(n);
  if (!p) die("malloc failed");
  return p;
}

static void fenwick_init(Fenwick *fw, int n) {
  fw->n = n;
  fw->bit = (mpz_t *)xmalloc((size_t)(n + 1) * sizeof(mpz_t));
  for (int i = 0; i <= n; ++i) mpz_init(fw->bit[i]);
}

static void fenwick_clear(Fenwick *fw) {
  for (int i = 0; i <= fw->n; ++i) mpz_clear(fw->bit[i]);
  free(fw->bit);
}

static void fenwick_add_ui(Fenwick *fw, int idx, unsigned long v, mpz_t active_total) {
  if (v == 0) return;
  mpz_add_ui(active_total, active_total, v);
  while (idx <= fw->n) {
    mpz_add_ui(fw->bit[idx], fw->bit[idx], v);
    idx += idx & -idx;
  }
}

static void fenwick_add_z(Fenwick *fw, int idx, const mpz_t v, mpz_t active_total) {
  if (mpz_sgn(v) == 0) return;
  mpz_add(active_total, active_total, v);
  while (idx <= fw->n) {
    mpz_add(fw->bit[idx], fw->bit[idx], v);
    idx += idx & -idx;
  }
}

static void fenwick_sum(Fenwick *fw, int idx, mpz_t out) {
  mpz_set_ui(out, 0);
  while (idx > 0) {
    mpz_add(out, out, fw->bit[idx]);
    idx -= idx & -idx;
  }
}

static IntVec *build_divisors(int nmax) {
  IntVec *divs = (IntVec *)xmalloc((size_t)(nmax + 1) * sizeof(IntVec));
  int *cnt = (int *)calloc((size_t)(nmax + 1), sizeof(int));
  if (!cnt) die("calloc failed");

  for (int d = 1; d <= nmax; ++d) {
    for (int m = d; m <= nmax; m += d) cnt[m]++;
  }

  for (int i = 0; i <= nmax; ++i) {
    divs[i].len = cnt[i];
    divs[i].v = cnt[i] ? (int *)xmalloc((size_t)cnt[i] * sizeof(int)) : NULL;
    cnt[i] = 0;
  }

  for (int d = 1; d <= nmax; ++d) {
    for (int m = d; m <= nmax; m += d) {
      divs[m].v[cnt[m]++] = d;
    }
  }

  free(cnt);
  return divs;
}

static void free_divisors(IntVec *divs, int nmax) {
  for (int i = 0; i <= nmax; ++i) free(divs[i].v);
  free(divs);
}

static BigRow **alloc_dp(int nmax) {
  BigRow **dp = (BigRow **)xmalloc((size_t)(nmax + 1) * sizeof(BigRow *));
  for (int n = 0; n <= nmax; ++n) {
    dp[n] = (BigRow *)calloc((size_t)(n + 1), sizeof(BigRow));
    if (!dp[n]) die("calloc dp row failed");
  }
  return dp;
}

static void free_dp(BigRow **dp, int nmax) {
  for (int n = 0; n <= nmax; ++n) {
    for (int s = 0; s <= n; ++s) {
      if (dp[n][s].a) {
        for (int i = 0; i < dp[n][s].len; ++i) mpz_clear(dp[n][s].a[i]);
        free(dp[n][s].a);
      }
    }
    free(dp[n]);
  }
  free(dp);
}

static void write_a_values(int nmax, const char *out_path) {
  FILE *out = fopen(out_path, "w");
  if (!out) {
    perror("fopen output");
    die("cannot open output file");
  }

  IntVec *divs = build_divisors(nmax);
  BigRow **dp = alloc_dp(nmax);

  fprintf(out, "0 1\n");

  mpz_t active_total, pref;
  mpz_init(active_total);
  mpz_init(pref);

  for (int n = 1; n <= nmax; ++n) {
    Fenwick fw;
    fenwick_init(&fw, n);
    mpz_set_ui(active_total, 0);

    for (int s_prev = n - 1; s_prev >= 0; --s_prev) {
      int s_add = s_prev + 1;
      int n2 = n - s_add;
      IntVec ds_add = divs[s_add];

      if (n2 == 0) {
        for (int i = 0; i < ds_add.len; ++i) {
          fenwick_add_ui(&fw, ds_add.v[i], 1UL, active_total);
        }
      } else if (s_add < n2) {
        BigRow prev = dp[n2][s_add];
        for (int i = 0; i < ds_add.len; ++i) {
          fenwick_add_z(&fw, ds_add.v[i], prev.a[i], active_total);
        }
      }

      if (s_prev == 0) {
        char *s = mpz_get_str(NULL, 10, active_total);
        if (!s) die("mpz_get_str failed");
        fprintf(out, "%d %s\n", n, s);
        free(s);
      } else {
        IntVec ds = divs[s_prev];
        BigRow row;
        row.len = ds.len;
        row.a = (mpz_t *)xmalloc((size_t)row.len * sizeof(mpz_t));

        for (int i = 0; i < row.len; ++i) {
          mpz_init(row.a[i]);
          fenwick_sum(&fw, ds.v[i], pref);
          mpz_sub(row.a[i], active_total, pref);
        }
        dp[n][s_prev] = row;
      }
    }

    fenwick_clear(&fw);
  }

  mpz_clear(active_total);
  mpz_clear(pref);
  free_dp(dp, nmax);
  free_divisors(divs, nmax);
  fclose(out);
}

int main(int argc, char **argv) {
  int nmax = 10000;
  if (argc >= 2) {
    nmax = atoi(argv[1]);
    if (nmax < 0) die("nmax must be >= 0");
  }

  write_a_values(nmax, "b304428_1.txt");
  return 0;
}
