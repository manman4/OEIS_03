M=19;

a(n, k=1) = if(k==0, 0^n, k*sum(j=0, n, binomial(n+j+k, j)/(n+j+k)*a(n-j, 5*j)));
for(n=0, M, print1(a(n),", "));

