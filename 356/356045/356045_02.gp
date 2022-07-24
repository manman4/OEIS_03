M=11;

T(n, k) = sum(j=1, n, sumdiv(j, d, d^k*numdiv(j/d)));
for(n=0, M, for(k=1, n, print1(T(k, n-k), ", ")));