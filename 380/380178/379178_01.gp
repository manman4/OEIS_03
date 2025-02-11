M=10;

a(n, k) = if(k==0, 0^n, k*sum(j=0, n, (n-j+k)^(j-1)*binomial(n, j)*a(n-j, j)));
for(n=0, M, for(k=0, n, print1(a(k, n-k),", ")))  