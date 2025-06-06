M=21;

a(n, k=1) = if(k==0, 0^n, k*sum(j=0, n, binomial(4*n-3*j+k, j)/(4*n-3*j+k)*a(n-j, j)));
for(n=0, M, print1(a(n),", "));

