M=10;

T(n, k) = polcoef(prod(j=0, n^k\2, 1+x^(2*j+1)+x*O(x^(n^k))), n^k);
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));