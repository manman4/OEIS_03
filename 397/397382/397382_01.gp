\\ a(n) = Sum_{k=0..n} binomial(n+3*k,4*k) * (binomial(n+4*k,k) + binomial(n+4*k,k-1)).
a(n) = sum(k=0, n, binomial(n+3*k,4*k) * (binomial(n+4*k,k) + binomial(n+4*k,k-1)));
for(n=0, 18, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n} binomial(n+3*k,4*k) * binomial(n+4*k,k).
b(n) = sum(k=0, n, binomial(n + 3*k, 4*k) * binomial(n+4*k, k));
\\ a(n) = Sum_{k=0..n} binomial(n+3*k,4*k) * binomial(n+4*k,k-1).
c(n) = sum(k=0, n, binomial(n+3*k,4*k) * binomial(n+4*k,k-1));
\\ a(n) = A397386(n) + A397385(n).
for(n=0, 50, print1(b(n)+c(n)-a(n), ", "));
