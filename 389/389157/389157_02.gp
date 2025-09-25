\\ a(n) = Sum_{k=0..n} binomial(n,k) * A389155(k).
a389155(n) = (1/(n+1)) * sum(k=0, n\2, binomial(n+1,k) * binomial(4*k,n-2*k));
a(n) = sum(k=0, n, binomial(n, k) * a389155(k));
for(n=0, 25, print1(a(n), ", ")); 