M=19;

a(n) = sum(k=0, n\2, 4^(n-k)*stirling(n, 2*k, 2));
for(n=0, M, print1(a(n), ", "));