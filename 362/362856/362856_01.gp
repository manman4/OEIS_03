M=9;

T(n, k) = sum(j=0, n, (-k)^(n-j)*j^j*binomial(n, j));
for(n=0, M, for(k=0, n, print1(T(k,n-k),", "))) 
