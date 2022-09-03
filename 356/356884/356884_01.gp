M=20;

a(n) = n!*sum(k=0, n\2, (n-2*k+1)^(k-1)*abs(stirling(n-k, k, 1))/(n-k)!);
for(n=0, M, print1(a(n), ", "));