M=22;

a(n) = n!*sum(k=1, n\4, k^(k-1)*abs(stirling(n-3*k, k, 1))/(n-3*k)!)/6;
for(n=0, M, print1(a(n), ", "));