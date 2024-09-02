M=18;

\\ a(n) = 2 * (n+1)! * Sum_{k=0..n} |Stirling1(n,k)|/(n-k+2)!.
a(n) = 2 * (n+1)! * sum(k=0, n, abs(stirling(n, k, 1)) / (n-k+2)!);
for(n=0, M, print1(a(n), ", "));
