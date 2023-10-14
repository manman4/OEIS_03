M=9;

T(n, k) = polcoef(prod(j=1, n^k, 1+x^j+x*O(x^(n^k))), n^k);
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));