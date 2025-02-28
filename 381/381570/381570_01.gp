M=19;

a(n, k=1) = if(k==0, 0^n, k*sum(j=0, n, binomial(3*n-3*j+3*k, j)/(n-j+k)*a(n-j, j)));
for(n=0, M, print1(a(n),", "));

