\\ a(n) = (-1)^(n-1) * (1/n) * Sum_{k=0..n} binomial(n,k) * binomial(3*n-4*k-2,n-1) for n > 0.
a(n) = if(n==0, 1, (-1)^(n-1) * (1/n) * sum(k=0, n, binomial(n, k) * binomial(3*n-4*k-2, n-1)));
for(n=0, 22, print1(a(n), ", "))
