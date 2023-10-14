M=11;

T(n, k) = n!*sum(j=0, n\k, j^(n-k*j)/(k!^j*(n-k*j)!));
for(n=0, M, for(k=0, n-1, print1(T(k,n-k),", "))) 