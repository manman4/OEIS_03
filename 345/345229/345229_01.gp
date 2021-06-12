M=12;

T(n, k) = sum(j=1, n, sumdiv(j, d, eulerphi(j/d)*binomial(d+k-2, k-1)));
for(n=1, M, for(k=1, n, print1(T(k, n-k+1), ", ")));