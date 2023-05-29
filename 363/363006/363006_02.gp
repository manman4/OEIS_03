a(n,i,j) = sum(k=0, n, binomial(n, k)*binomial(i*n+j*k+1, n)/(i*n+j*k+1) );

for(n=0, 19, print1(a(n,5,1), ", ")) 
