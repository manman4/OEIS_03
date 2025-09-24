\\ a(n) = Sum_{k=0..n} binomial(n,k) * A389129(k).
a389129(n) = (1/(n+1)) * sum(k=0, n\3, binomial(n+1,k) * binomial(2*k,n-3*k));
a(n) = sum(k=0, n, binomial(n, k) * a389129(k));
for(n=0, 30, print1(a(n), ", ")); 