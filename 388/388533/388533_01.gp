\\ a(n) = Sum_{k=0..n} binomial(n,k) * binomial(n+2*k,k).
a(n) = sum(k=0, n, binomial(n, k)*binomial(n+2*k, k));
for(n=0, 21, print1(a(n), ", ")); 

\\ a(n) = Sum_{k=0..n} binomial(n,k) * A370286(k).
a370286(n) = sum(k=0, n\2, binomial(n, k)*binomial(3*n-3*k, n-2*k));
b(n) = sum(k=0, n, binomial(n, k)*a370286(k));
for(n=0, 100, print1(a(n) - b(n), ", "));