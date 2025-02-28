M=21;

a(n, k=1) = if(k==0, 0^n, 2*k*sum(j=0, n, binomial(2*n-j+2*k, j)/(2*n-j+2*k)*a(n-j, j)));
for(n=0, M, print1(a(n),", "));

