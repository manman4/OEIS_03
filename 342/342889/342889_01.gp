M=8;

f(n, k, m) = prod(j=1, k, binomial(n-j+m, m)/binomial(j-1+m, m));
T(n, k) = f(n, k, 10); 
for(n=0, M, for(k=0, n, print1(T(n, k), ", ")))