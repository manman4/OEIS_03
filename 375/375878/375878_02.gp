M=18;

\\ a(n) = 2 * Sum_{k=0..n} (k+2)^(k-1) * |Stirling1(n,k)|.
a(n) = 2 * sum(k=0, n, (k+2)^(k-1) * abs(stirling(n, k, 1)));
for(n=0, M, print1(a(n), ", "));
