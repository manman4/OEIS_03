a090965(n) = 2^n*polchebyshev(n, 1, 2);
for(n=0, 12, print1(a090965(n),", "));
\\ a(n) = (-1)^n * A090965(n) - Sum_{k=0..n-1} (-9)^(n-k) * binomial(2*n,2*k) * a(k).
a(n) = (-1)^n * a090965(n) - sum(k=0, n-1, (-9)^(n-k) * binomial(2*n,2*k) * a(k));
for(n=0, 12, print1(a(n),", "));
