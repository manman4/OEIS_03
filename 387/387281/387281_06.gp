\\ a(n) = Sum_{k=0..floor(n/2)} 4^(n-2*k) * binomial(n+3,n-2*k) * binomial(2*k+3,k).
a387273(n) = sum(k=0, n\2, 4^(n-2*k) * binomial(n+3,n-2*k) * binomial(2*k+3,k));

\\ a(n) = (binomial(n+6,3)/20) * A387273(n).
a(n) = (binomial(n+6,3)/20) * a387273(n);
for(n=0, 20, print1(a(n), ", "));
