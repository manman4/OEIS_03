\\ a(n) = Sum_{k=0..floor(n/2)} 2^k * 3^(n-2*k) * binomial(n+2,n-2*k) * binomial(2*k+2,k).
a387337(n) = sum(k=0, n\2, 2^k * 3^(n-2*k) * binomial(n+2,n-2*k) * binomial(2*k+2,k));

\\ a(n) = (binomial(n+4,2)/6) * A387337(n).
a(n) = (binomial(n+4,2)/6) * a387337(n);
for(n=0, 20, print1(a(n), ", "));