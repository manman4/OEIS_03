M=20;

a(n, k=1) = if(k==0, 0^n, k*sum(j=0, n, binomial(5*n-4*j+k, j)/(5*n-4*j+k)*a(n-j, j)));
for(n=0, M, print1(a(n),", "));

