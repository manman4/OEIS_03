\\ a(n) = Sum_{k=0..2*n} (-1)^k * Stirling2(2*n+k,2*n) * Stirling2(4*n-k,2*n).
a(n) = sum(k=0, 2*n, (-1)^k * stirling(2*n+k, 2*n, 2) * stirling(4*n-k, 2*n, 2));
for(n=0, 20, print1(a(n),", "));
