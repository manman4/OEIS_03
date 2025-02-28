M=10;

a(n, k) = if(k==0, 0^n, 2*k*sum(j=0, n, binomial(2*n-j+2*k, j)/(2*n-j+2*k)*a(n-j, j)));
for(n=0, M, for(k=0, n, print1(a(k, n-k),", "))) 

