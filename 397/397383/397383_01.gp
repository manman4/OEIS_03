\\ a(n) = Sum_{k=0..n} binomial(n+k,2*k) * (binomial(n+2*k,k) + binomial(n+2*k,k-1)).
a(n) = sum(k=0, n, binomial(n+k,2*k) * (binomial(n+2*k,k) + binomial(n+2*k,k-1)));
for(n=0, 20, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n} C(n + 2*k, 3*k) * C(3*k, 2*k).
b(n) = sum(k=0, n, binomial(n + 2*k, 3*k) * binomial(3*k, 2*k));
\\ a(n) = Sum_{k=0..n} binomial(n+k,2*k) * binomial(n+2*k,k-1).
c(n) = sum(k=0, n, binomial(n+k,2*k) * binomial(n+2*k,k-1));
\\ a(n) = A243116(n) + A397383(n).
for(n=0, 50, print1(b(n)+c(n)-a(n), ", "));
