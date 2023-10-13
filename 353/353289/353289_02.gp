M=22;

a(n) = sum(k=0, n\2, (n-k)^k*abs(stirling(n-k, k, 1)));
for(n=0, M, print1(a(n), ", "));