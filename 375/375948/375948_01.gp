M=18;

a001147(n) = prod(k=0, n-1, 2*k+1);
for(n=0, M, print1(a001147(n), ", "));

\\ a(n) = Sum_{k=0..n} A001147(k+1) * Stirling2(n,k).
a(n) = sum(k=0, n, a001147(k+1) * stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));
