M=9;

a(n, k) = if(k==0, 0^n, 3*k*sum(j=0, n, binomial(3*n-2*j+3*k, j)/(3*n-2*j+3*k)*a(n-j, j)));
for(n=0, M, for(k=0, n, print1(a(k, n-k),", "))) 

