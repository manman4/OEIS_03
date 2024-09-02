M=17;

\\ a(n) = (3/(n+3)!) * Sum_{k=0..n} (n+k+2)! * Stirling2(n,k).
a(n) = 3 * sum(k=0, n, (n+k+2)! * stirling(n, k, 2) / (n+3)!);
for(n=0, M, print1(a(n), ", "));
