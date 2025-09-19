\\ a(n) = Sum_{k=0..floor(n/4)} binomial(n,k) * binomial(n,4*k).
a(n) = sum(k=0, n\4, binomial(n, k)*binomial(n, 4*k));
for(n=0, 30, print1(a(n), ", ")); 

\\ a(n) = Sum_{k=0..n} binomial(n,k) * A387200(k).
a387200(n) = sum(k=0, n\4, binomial(n, k)*binomial(k, n-4*k));
b(n) = sum(k=0, n, binomial(n, k)*a387200(k));
for(n=0, 100, print1(a(n) - b(n), ", "));