\\ a(n) = 4 * Sum_{k=0..n} binomial(k+4,4) * binomial(3*n-2*k+4,n-k)/(3*n-2*k+4).
a(n) = 4 * sum(k=0, n, binomial(k+4,4)*binomial(3*n-2*k+4, n-k)/(3*n-2*k+4));
for(n=0, 22, print1(a(n), ", "));



