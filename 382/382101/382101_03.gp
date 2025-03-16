M=10;

\\ A(n,k) = (n-1)! * Sum_{j=0..n-1} binomial(k*n,j)/(n-j-1)! for n > 0.
a(n, k) = if(n==0, 1, (n-1)! * sum(j=0, n-1, binomial(k*n,j)/((n-j-1)!)));
for(n=0, M, for(k=0, n, print1(a(k,n-k),", "))) 


