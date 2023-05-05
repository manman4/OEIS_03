M=10;

T(n, k) = polcoef(sum(j=0, n, x^j / (1 - (k*j-1)*x)^(j+1) + x*O(x^n)), n);
for(n=0, M, for(k=0, n, print1(T(k,n-k),", "))) 
