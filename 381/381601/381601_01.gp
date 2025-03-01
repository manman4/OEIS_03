M=18;

a(n, k=1) = if(k==0, 0^n, k*sum(j=0, n, binomial(3*n+j+k, j)/(3*n+j+k)*a(n-j, 3*j)));
for(n=0, M, print1(a(n),", "));

