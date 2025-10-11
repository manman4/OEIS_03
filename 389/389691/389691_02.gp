\\ a(n) = (1/(n+1)) * Sum_{k=0..n} binomial(2*n+k+1,k) * binomial(n+k-1,n-k).
a(n) = (1/(n+1)) * sum(k=0, n, binomial(2*n+k+1,k) * binomial(n+k-1,n-k) );
for(n=0, 21, print1(a(n),", "))   


