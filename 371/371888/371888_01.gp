\\ a(n) = (1/n) * Sum_{k=0..n} binomial(n,k) * binomial(n-2*k,n-k-1) for n > 0.
C(n, k) = if(k>=0, binomial(n, k));
a(n) = if(n==0, 1, sum(k=0, n, C(n, k)*C(n-2*k, n-k-1))/n);
for(n=0, 36, print1(a(n), ", "))
