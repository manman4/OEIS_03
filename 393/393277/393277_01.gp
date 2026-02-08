\\ a(n) = Sum_{k=0..floor(n/4)} binomial(2*k,k) * binomial(n-2*k-1,n-4*k).
a(n) = sum(k=0, n\4, binomial(2*k, k)*binomial(n-2*k-1, n-4*k));
for(n=0, 38, print1(a(n),", "));

\\ a(n) = Sum_{k=0..floor(n/4)} binomial(n-2*k, k) * binomial(n-3*k, k).
a098482(n) = sum(k=0, n\4, binomial(n-2*k, k)*binomial(n-3*k, k));
\\ Partial sums are A098482.
for(n=0, 40, print1(sum(k=0, n , a(k))-a098482(n),", "));