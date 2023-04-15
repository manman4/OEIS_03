M=11;

T(n, k) = n!*sum(j=0, n\3, (k/6)^j*binomial(n-2*j, j)/(n-2*j)!);
for(n=0, M, for(k=0, n, print1(T(k,n-k), ", "))); 