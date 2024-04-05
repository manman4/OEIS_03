\\ a(n) = binomial(5*n,n) - 3 * Sum_{k=0..n-1} binomial(5*n,k).
a(n) = binomial(5*n,n) - 3 * sum(k=0, n-1, binomial(5*n,k));
for(n=0, 20, print1(a(n),", "))