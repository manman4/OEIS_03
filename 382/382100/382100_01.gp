M=10;
a(n, k) = polcoef(1/(2-sum(j=0, n, binomial(k*j+1, j)/(k*j+1)*x^j+x*O(x^n))), n);
for(n=0, M, for(k=0, n, print1(a(k,n-k),", "))) 


