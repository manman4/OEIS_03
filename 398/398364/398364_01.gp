\\ a(n) = Sum_{k=0..floor(n/7)} binomial(2*k,k) * binomial(n-5*k,2*k).
a(n) = sum(k=0, n\7, binomial(2*k, k)*binomial(n-5*k, 2*k));
for(n=0, 44, print1(a(n),", "));

\\ a(n) = Sum_{k=0..floor(n/7)} multinomial(n-5*k; k, k, n-7*k).
b(n) = sum(k=0, n\7, (n-5*k)!/(k!*k!*(n-7*k)!));
for(n=0, 40, print1(a(n)-b(n),", "));