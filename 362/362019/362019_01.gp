M=9;

T(n, k) = (-1)^n*sum(j=0, n, (-k*j)^j*binomial(n, j));
for(n=0, M, for(k=0, n, print1(T(k,n-k),", "))) 
