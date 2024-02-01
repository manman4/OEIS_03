bell(n, x) = sum(k=0, n, x^k * stirling(n, k, 2));
a247452(n) = 3^n * bell(n, 1);
for(n=0, 18, print1(a247452(n), ", "));
\\ Sum_{k=0..n} Stirling2(n,k) * A247452(k)
a(n) = sum(k=0, n, stirling(n, k, 2) * a247452(k));
for(n=0, 18, print1(a(n), ", "));