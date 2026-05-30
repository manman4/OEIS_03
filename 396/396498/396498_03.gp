/*
  Square array A(n, k), n >= 0, k >= 0, read by antidiagonals downward.

  A(n, k) = Sum_{0 = x_0 <= x_1 <= ... <= x_{k-1} <= x_k = n}
              Product_{j=0..k-1}
                (x_j + 1) * (x_{j+1} + 1)^(x_{j+1} - x_j - 1) * binomial(x_{j+1}, x_j).

  Equivalent recurrence:

    A(0, 0) = 1,
    A(n, 0) = 0 for n > 0,
    A(n, k) = Sum_{j=0..n} A(j, k-1) * binomial(n, j) * (j + 1) * (n + 1)^(n - j - 1)
    for n >= 0, k >= 1.
*/

default_n = 15;
default_k = 15;

/*
  Return A(n, k).
  Computed by dynamic programming from the recurrence above.
*/
a(n, k) = {
  my(tab, nn, kk, j);

  if(n < 0 || k < 0,
    return(0);
  );

  tab = matrix(n + 1, k + 1, i, j, 0);
  tab[1, 1] = 1;

  for(kk = 2, k + 1,
    tab[1, kk] = 1;
    for(nn = 2, n + 1,
      tab[nn, kk] = sum(j = 0, nn - 1,
        tab[j + 1, kk - 1] * binomial(nn - 1, j) * (j + 1) * nn^(nn - j - 1)
      );
    );
  );

  tab[n + 1, k + 1]
};

for(k = 0, 15, print1(a(1, k), ", "));
for(k = 0, 15, print1(a(2, k), ", "));
for(k = 0, 15, print1(a(3, k), ", "));
for(k = 0, 15, print1(a(4, k), ", "));
for(k = 0, 15, print1(a(5, k), ", "));

matrix(7, 7, n, k, a(n - 1, k - 1))

for(n = 0, 10, for(k = 0, n, print1(a(k, n - k), ", ")));

\\ A(n,0) = 0^n; A(n,k) = Sum_{j=0..n} (j+1) * (n+1)^(n-j-1) * binomial(n,j) * A(j,k-1) for k > 0.
a(n,k) = if(k==0, 0^n, sum(j=0, n, (j+1) * (n+1)^(n-j-1) * binomial(n,j) * a(j,k-1)));
for(n=0, 10, for(k=0, n, print1(a(k, n-k), ", ")));