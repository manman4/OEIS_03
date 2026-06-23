\\ a(n) = Sum_{k=0..n} binomial(n+2*k,3*k) * (binomial(n+3*k,k) - binomial(n+3*k,k-1)).
a(n) = sum(k=0, n, binomial(n+2*k,3*k) * (binomial(n+3*k,k) - binomial(n+3*k,k-1)));
for(n=0, 20, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n} binomial(n,k) * binomial(n+3*k,n).
b(n) = sum(k=0, n, binomial(n,k) * binomial(n+3*k,n));
\\ a(n) = Sum_{k=0..n} binomial(n+2*k,3*k) * binomial(n+3*k,k-1).
c(n) = sum(k=0, n, binomial(n+2*k,3*k) * binomial(n+3*k,k-1));
\\ a(n) = A378402(n) - A397384(n).
for(n=0, 50, print1(b(n)-c(n)-a(n), ", "));
