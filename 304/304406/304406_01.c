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

static void fenwick_add_z(Fenwick *fw, int idx, const mpz_t delta) {
  if (mpz_sgn(delta) == 0) return;
  while (idx <= fw->n) {
    mpz_add(fw->bit[idx], fw->bit[idx], delta);
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
    for (int m = d; m <= nmax; m += d) divs[m].v[cnt[m]++] = d;
  }

  free(cnt);
  return divs;
}

static void free_divisors(IntVec *divs, int nmax) {
  for (int i = 0; i <= nmax; ++i) free(divs[i].v);
  free(divs);
}

static BigRow **alloc_dp_sparse(int nmax) {
  BigRow **dp = (BigRow **)xmalloc((size_t)(nmax + 1) * sizeof(BigRow *));
  for (int n = 0; n <= nmax; ++n) {
    dp[n] = (BigRow *)calloc((size_t)(n + 1), sizeof(BigRow));
    if (!dp[n]) die("calloc failed");
  }
  return dp;
}

static void free_dp_sparse(BigRow **dp, int nmax) {
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

static mpz_t **alloc_dp_full(int nmax) {
  mpz_t **dp = (mpz_t **)xmalloc((size_t)(nmax + 1) * sizeof(mpz_t *));
  for (int n = 0; n <= nmax; ++n) {
    dp[n] = (mpz_t *)xmalloc((size_t)(n + 2) * sizeof(mpz_t));
    for (int d = 0; d <= n + 1; ++d) mpz_init(dp[n][d]);
  }
  return dp;
}

static void free_dp_full(mpz_t **dp, int nmax) {
  for (int n = 0; n <= nmax; ++n) {
    for (int d = 0; d <= n + 1; ++d) mpz_clear(dp[n][d]);
    free(dp[n]);
  }
  free(dp);
}

static void compute_and_write(int nmax, const char *out_path) {
  FILE *out = fopen(out_path, "w");
  if (!out) {
    perror("fopen output");
    die("cannot open output file");
  }

  IntVec *divs = build_divisors(nmax);
  BigRow **dp_sparse = alloc_dp_sparse(nmax);
  mpz_t **dp_full = alloc_dp_full(nmax);

  fprintf(out, "0 1\n");

  mpz_t one, active_total, pref, d_weight;
  mpz_init_set_ui(one, 1);
  mpz_init(active_total);
  mpz_init(pref);
  mpz_init(d_weight);

  for (int n = 1; n <= nmax; ++n) {
    Fenwick fw;
    fenwick_init(&fw, n);
    mpz_set_ui(active_total, 0);

    for (int s = 1; s <= n; ++s) {
      IntVec ds = divs[s];
      int n2 = n - s;

      for (int i = 0; i < ds.len; ++i) {
        int d = ds.v[i];

        if (n2 == 0) {
          mpz_set(d_weight, one);
        } else if (s < n2) {
          BigRow prev = dp_sparse[n2][s];
          if (prev.a) {
            mpz_set(d_weight, prev.a[i]);
          } else {
            mpz_set_ui(d_weight, 0);
          }
        } else {
          int d2 = d + 1;
          if (d2 <= n2 + 1) {
            mpz_set(d_weight, dp_full[n2][d2]);
          } else {
            mpz_set_ui(d_weight, 0);
          }
        }

        if (mpz_sgn(d_weight) != 0) {
          fenwick_add_z(&fw, d, d_weight);
          mpz_add(active_total, active_total, d_weight);
        }
      }

      if (s < n) {
        BigRow row;
        row.len = ds.len;
        row.a = (mpz_t *)xmalloc((size_t)row.len * sizeof(mpz_t));

        for (int i = 0; i < row.len; ++i) {
          mpz_init(row.a[i]);
          int d = ds.v[i];
          fenwick_sum(&fw, d, pref);
          mpz_sub(row.a[i], active_total, pref);
        }

        dp_sparse[n][s] = row;
      } else {
        fenwick_sum(&fw, 0, pref);
        (void)pref;

        mpz_t ans;
        mpz_init(ans);
        fenwick_sum(&fw, 0, ans);
        mpz_sub(ans, active_total, ans);
        mpz_set(dp_full[n][1], ans);

        for (int dmin = 2; dmin <= n + 1; ++dmin) {
          fenwick_sum(&fw, dmin - 1, pref);
          mpz_sub(dp_full[n][dmin], active_total, pref);
        }

        char *sval = mpz_get_str(NULL, 10, dp_full[n][1]);
        if (!sval) die("mpz_get_str failed");
        fprintf(out, "%d %s\n", n, sval);
        free(sval);
        mpz_clear(ans);
      }
    }

    fenwick_clear(&fw);
  }

  mpz_clear(one);
  mpz_clear(active_total);
  mpz_clear(pref);
  mpz_clear(d_weight);

  free_dp_sparse(dp_sparse, nmax);
  free_dp_full(dp_full, nmax);
  free_divisors(divs, nmax);
  fclose(out);
}

int main(int argc, char **argv) {
  int nmax = 5000;
  const char *out_path = "b304406_1.txt";

  if (argc >= 2) nmax = atoi(argv[1]);
  if (argc >= 3) out_path = argv[2];
  if (nmax < 0) die("nmax must be >= 0");

  compute_and_write(nmax, out_path);
  return 0;
}
