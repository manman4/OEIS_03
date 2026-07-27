\\ a(n) = Sum_{k=0..floor(n/8)} binomial(2*k,k) * binomial(n-6*k,2*k).
a(n) = sum(k=0, n\8, binomial(2*k, k)*binomial(n-6*k, 2*k));
for(n=0, 45, print1(a(n),", "));

\\ a(n) = Sum_{k=0..floor(n/8)} multinomial(n-6*k; k, k, n-8*k).
b(n) = sum(k=0, n\8, (n-6*k)!/(k!*k!*(n-8*k)!));
for(n=0, 40, print1(a(n)-b(n),", "));