\\ a(n) = (-1)^(n-1) * (1/n) * Sum_{k=0..n} binomial(n,k) * binomial(2*n-3*k-2,n-1) for n > 0.
a(n) = if(n==0, 1, (-1)^(n-1) * sum(k=0, n, binomial(n,k) * binomial(2*n-3*k-2,n-1) )/n);
for(n=0, 50, print1(a(n), ", "))     
