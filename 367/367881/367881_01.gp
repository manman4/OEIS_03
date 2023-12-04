a(n) = n!*sum(k=0, n\2, 3^k*k!*stirling(n-k, k, 2)/(n-k)!);
for(n=0, 19, print1(a(n), ", "));