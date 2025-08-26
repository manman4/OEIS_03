\\ a(n) = Sum_{k=0..floor(n/2)} 4^k * 5^(n-2*k) * binomial(n+2,n-2*k) * binomial(2*k+2,k).
a387307(n) = sum(k=0, n\2, 4^k * 5^(n-2*k) * binomial(n+2,n-2*k) * binomial(2*k+2,k));

\\ a(n) = (binomial(n+4,2)/6) * A387307(n).
a(n) = (binomial(n+4,2)/6) * a387307(n);
for(n=0, 20, print1(a(n), ", "));