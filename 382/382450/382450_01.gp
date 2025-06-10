M=16;

a(n, k=1) = if(k==0, 0^n, k*sum(j=0, n, 2^(n-j)*binomial(n+k, j)/(n+k)*a(n-j, j)));
for(n=0, M, print1(a(n),", "));

