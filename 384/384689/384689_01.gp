M=17;

a(n, k=1) = if(k==0, 0^n, k*sum(j=0, n, (n+j+k)^(j-1)*binomial(n, j)*a(n-j, j)));
for(n=0, M, print1(a(n),", "));

