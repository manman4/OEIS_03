M=19;

a(n, k=1) = if(k==0, 0^n, 3*k*sum(j=0, n, binomial(3*n-2*j+3*k, j)/(3*n-2*j+3*k)*a(n-j, j)));
for(n=0, M, print1(a(n),", "));

