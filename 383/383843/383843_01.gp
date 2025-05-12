\\ A(n,k) = Sum_{j=0..n} Stirling2(j+k,k) * Stirling2(n-j+k,k).
a(n, k) = sum(j=0, n, stirling(j+k, k, 2) * stirling(n-j+k, k, 2));
for(n=0, 10, for(k=0, n, print1(a(k, n-k),", ")));


