bell(n, x) = sum(k=0, n, x^k * stirling(n, k, 2));
a004211(n) = 2^n * bell(n, 1/2);
for(n=0, 18, print1(a004211(n), ", "));
\\ a(n) = Sum_{k=0..n} Stirling2(n,k) * A004211(k).
a(n) = sum(k=0, n, stirling(n, k, 2) * a004211(k));
for(n=0, 20, print1(a(n), ", "));