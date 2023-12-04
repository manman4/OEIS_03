a(n) = n!*sum(k=0, n\2, 2^(n-k)*k!*abs(stirling(n-k, k, 1))/(n-k)!);
for(n=0, 18, print1(a(n), ", "));