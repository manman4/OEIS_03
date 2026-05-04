a036829(n) = sum(k=0, n-1, binomial(3*k-2, k)*binomial(3*n-3*k, n-k-1));
for(n=0, 25, print1(a036829(n), ", "));
\\ a(n) = (n+1) * A036829(n+1).
a(n) = (n+1) * a036829(n+1);
for(n=0, 25, print1(a(n), ", "));



