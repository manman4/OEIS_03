M=10; 

a(n, k) = if(k==0, 0^n, k*sum(j=0, n, binomial(n+j+k, j)/(n+j+k)*a(n-j, j)));
for(n=0, M, for(k=0, n, print1(a(k, n-k),", ")))
