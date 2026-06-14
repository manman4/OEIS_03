\\ Let a(n,k) = n! * [x^n] f(x)^k.
a_egf(n, k) = {f = 1 + x * O(x);
  for(i = 1, n, f = exp(serreverse(x * f)));
  f^k
};
Vec(serlaplace(a_egf(10, 1)))
b(n, k) = n! * polcoef(a_egf(n, k), n);
matrix(7, 7, n, k, b(n-1, k-1))

\\ A(n,k) = k^n - Sum_{j=1..n-1} binomial(n,j) * A(j,k) * A(n-j,j).
a(n, k) = k^n-sum(j=1, n-1, binomial(n, j)*a(j, k)*a(n-j, j));
for(n=0, 10, for(k=0, n, print1(a(k,n-k),", ")));
\\ 一致の確認
for(n=0, 10, for(k=0, n, print1(a(k,n-k)-b(k,n-k),", ")));

