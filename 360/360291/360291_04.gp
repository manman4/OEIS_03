M=20;

a(n) = sum(k=0, n\3, binomial(n-2*k, k)*binomial(2*(n-3*k), n-3*k));
\\ a(n) = A383581(n) - A383581(n-3).
for(n=0, M, print1(a(n) - a(n-3), ", "));

