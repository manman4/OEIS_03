M=20;

a(n, k=1) = if(k==0, 0^n, k*sum(j=0, n, binomial(2*n+j+k, j)/(2*n+j+k)*a(n-j, 2*j)));
for(n=0, M, print1(a(n),", "));

