M=19;

a(n) = sum(k=0, n, (n-2*k+1)^(k-1)*stirling(n, k, 2));
for(n=0, M, print1(a(n), ", "));