a004212(n) = sum(k=0, n, 3^(n-k)*stirling(n, k, 2));

\\ a(n) = Sum_{k=0..n} 9^(n-k) * Stirling2(n,k) * A004212(k)
a(n) = sum(k=0, n, 9^(n-k) * stirling(n, k, 2) * a004212(k));
for(n=0, 17, print1(a(n), ", "));

bell(n, x) = sum(k=0, n, x^k * stirling(n, k, 2));

\\ 9^n * Sum_{k=0..n} (1/3)^k * Stirling2(n,k) * Bell_k(1/3)
b(n) = 9^n * sum(k=0, n, 1/3^k * stirling(n, k, 2) * bell(k, 1/3));
for(n=0, 50, print1(a(n)-b(n), ", "));
