M=20;

a(n) = sum(k=0, n\4, binomial(n-3*k, k)*binomial(2*(n-4*k), n-4*k));
\\ a(n) = A383582(n) - A383582(n-4).
for(n=0, M, print1(a(n) - a(n-4), ", "));

