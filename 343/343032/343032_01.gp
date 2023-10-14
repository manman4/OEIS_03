M=20;

a(n)=sum(k=0,n,sqrtint(prod(j=0,k-1,binomial(2*n-2*k+j, n-k+j)/(binomial(n-k+j, j)))*prod(j=0,n-k-1,(k+2*j+1)/(2*j+1))));
for(n=0, M, print1(a(n), ", "))  