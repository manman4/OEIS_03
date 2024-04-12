\\ a(n) = (1/n) * Sum_{k=0..n} binomial(n,k) * binomial(3*n-5*k,n-k-1) for n > 0.
a(n) = if(n==0, 1, (1/n) * sum(k=0, n, binomial(n, k) * binomial(3*n-5*k, n-k-1)));
for(n=0, 23, print1(a(n), ", "))
