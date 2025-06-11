M=18;

a(n, k=-1) = if(n*k==0, 0^n, (-1)^n*k*sum(j=1, n, (-2*n+2*j+k)^(j-1)*binomial(n, j)*a(n-j, 3*j)));
for(n=0, M, print1(a(n),", "));

