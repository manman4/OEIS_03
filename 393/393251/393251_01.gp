\\ a(n) = Sum_{k=0..floor(n/6)} binomial(2*k,k) * binomial(n-4*k,2*k).
a(n) = sum(k=0, n\6, binomial(2*k, k)*binomial(n-4*k, 2*k));
for(n=0, 40, print1(a(n),", "));

\\ a(n) = Sum_{k=0..floor(n/6)} multinomial(n-4*k; k, k, n-6*k).
b(n) = sum(k=0, n\6, (n-4*k)!/(k!*k!*(n-6*k)!));
for(n=0, 40, print1(a(n)-b(n),", "));