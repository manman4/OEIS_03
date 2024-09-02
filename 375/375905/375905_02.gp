M=18;

\\ a(n) = 3 * (n+2)! * Sum_{k=0..n} |Stirling1(n,k)|/(n-k+3)!.
a(n) = 3 * (n+2)! * sum(k=0, n, abs(stirling(n, k, 1)) / (n-k+3)!);
for(n=0, M, print1(a(n), ", "));
