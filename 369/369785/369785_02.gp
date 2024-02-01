bell(n, x) = sum(k=0, n, x^k * stirling(n, k, 2));
a004212(n) = 3^n * bell(n, 1/3);
for(n=0, 18, print1(a004212(n), ", "));
\\ a(n) = Sum_{k=0..n} Stirling2(n,k) * A004212(k).
a(n) = sum(k=0, n, stirling(n, k, 2) * a004212(k));
for(n=0, 19, print1(a(n), ", "));