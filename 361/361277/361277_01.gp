M=10;

T(n, k) = n!*sum(j=0, n, binomial(k*j, n-j)/j!);
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));