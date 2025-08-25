\\ a(n) = Sum_{k=0..floor(n/2)} 4^(n-2*k) * binomial(n+4,n-2*k) * binomial(2*k+4,k).
a387274(n) = sum(k=0, n\2, 4^(n-2*k) * binomial(n+4,n-2*k) * binomial(2*k+4,k));

\\ a(n) = (binomial(n+8,4)/70) * A387274(n).
a(n) = (binomial(n+8,4)/70) * a387274(n);
for(n=0, 20, print1(a(n), ", "));