M=18;

a(n) = n!*sum(k=0, n\2, 3^(n-2*k)*k!*abs(stirling(n-k, k, 1))/(n-k)!);
for(n=0, M, print1(a(n), ", "));
