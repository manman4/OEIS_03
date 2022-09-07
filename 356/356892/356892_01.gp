M=21;

a(n) = n!*sum(k=0, n\4, (n-2*k+1)^(k-1)*stirling(n-3*k, k, 2)/(n-3*k)!);
for(n=0, M, print1(a(n), ", "));