\\ a(n) = (1/(n+1)) * Sum_{k=0..n} binomial(n+1,k) * binomial(3*n+2*k+1,n-k).
a(n) = (1/(n+1)) * sum(k=0, n, binomial(n+1,k) * binomial(3*n+2*k+1,n-k) );
for(n=0, 21, print1(a(n),", "))   


