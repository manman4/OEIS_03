M=10;

\\ A(n,k) = ( binomial(k*n,n) - (k-2) * Sum_{j=0..n-1} binomial(k*n,j) )/2 for n > 0.
a(n, k) = if(n==0, 1, (binomial(k*n,n) - (k-2) * sum(j=0, n-1, binomial(k*n,j)))/2);
for(n=0, M, for(k=0, n, print1(a(k,n-k),", "))) 


