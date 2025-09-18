\\ a(n) = Sum_{k=0..n} binomial(n,k) * binomial(n+3*k,k).
a(n) = sum(k=0, n, binomial(n, k)*binomial(n+3*k, k));
for(n=0, 20, print1(a(n), ", ")); 

\\ a(n) = Sum_{k=0..n} binomial(n,k) * A388530(k).
a388530(n) = sum(k=0, n\2, binomial(n, k)*binomial(4*n-4*k, n-2*k));
b(n) = sum(k=0, n, binomial(n, k)*a388530(k));
for(n=0, 100, print1(a(n) - b(n), ", "));