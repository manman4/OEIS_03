M=21;

a(n) = n!*sum(k=1, n\3, k^(k-1)*stirling(n-2*k, k, 2)/(n-2*k)!)/2;
for(n=0, M, print1(a(n), ", "));