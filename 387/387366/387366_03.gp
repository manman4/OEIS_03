\\ a(n) = Sum_{k=0..floor(n/2)} 6^k * 5^(n-2*k) * binomial(n+1,n-2*k) * binomial(2*k+1,k).
a387368(n) = sum(k=0, n\2, 6^k * 5^(n-2*k) * binomial(n+1,n-2*k) * binomial(2*k+1,k));

\\ a(n) = ((n+2)/2) * A387368(n).
a(n) = ((n+2)/2) * a387368(n);
for(n=0, 20, print1(a(n), ", "));