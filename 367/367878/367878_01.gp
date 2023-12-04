a(n) = n!*sum(k=0, n\2, 2^k*k!*abs(stirling(n-k, k, 1))/(n-k)!);
for(n=0, 19, print1(a(n), ", "));