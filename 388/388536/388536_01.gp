\\ a(n) = Sum_{k=0..n} binomial(n,k) * binomial(n+5*k,k).
a(n) = sum(k=0, n, binomial(n, k)*binomial(n+5*k, k));
for(n=0, 19, print1(a(n), ", ")); 

\\ a(n) = Sum_{k=0..n} binomial(n,k) * A388537(k).
a388537(n) = sum(k=0, n\2, binomial(n, k)*binomial(6*n-6*k, n-2*k));
b(n) = sum(k=0, n, binomial(n, k)*a388537(k));
for(n=0, 100, print1(a(n) - b(n), ", "));