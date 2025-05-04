M=24;

\\ a(n) = Sum_{k=0..n} 4^k * (-5)^(n-k) * binomial(n+3,k+3) * Stirling2(k+3,3).
a(n) = sum(k=0, n, 4^k * (-5)^(n-k) * binomial(n+3, k+3) * stirling(k+3, 3, 2));
for(n=0, M, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n} (-4)^k * 7^(n-k) * binomial(n+3,k+3) * Stirling2(k+3,3).
b(n) = sum(k=0, n, (-4)^k * 7^(n-k) * binomial(n+3, k+3) * stirling(k+3, 3, 2));
for(n=0, 100, print1(a(n)-b(n), ", "));
