M=24;

a(n) = sum(k=0, n\2, k^k*stirling(n-k, k, 2));
for(n=0, M, print1(a(n), ", "));