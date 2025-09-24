\\ a(n) = Sum_{k=0..n} binomial(n,k) * A389130(k).
a389130(n) = (1/(n+1)) * sum(k=0, n\4, binomial(n+1,k) * binomial(2*k,n-4*k));
a(n) = sum(k=0, n, binomial(n, k) * a389130(k));
for(n=0, 31, print1(a(n), ", ")); 