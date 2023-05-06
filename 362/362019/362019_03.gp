M=9;

T(n, k) = polcoef(sum(j=0, n, (k*j*x)^j / (1 + x + x*O(x^n))^(j+1)), n);
for(n=0, M, for(k=0, n, print1(T(k,n-k),", "))) 
