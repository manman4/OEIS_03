M=20;

\\ a(n) = 3 * (n+2)! * Sum_{k=0..n} (-1)^k * Stirling2(n,k)/(n-k+3)!.
a(n) = 3 * (n+2)! * sum(k=0, n, (-1)^k * stirling(n, k, 2) / (n-k+3)!);
for(n=0, M, print1(a(n), ", "));
