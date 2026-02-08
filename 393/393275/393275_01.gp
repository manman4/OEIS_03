\\ a(n) = Sum_{k=0..floor(n/4)} binomial(2*k,k) * binomial(n-1,n-4*k).
a(n) = sum(k=0, n\4, binomial(2*k, k)*binomial(n-1, n-4*k));
for(n=0, 34, print1(a(n),", "));

\\ a(n) = Sum_{k=0..floor(n/4)} binomial(2*k,k) * binomial(n,4*k).
a393245(n) = sum(k=0, n\4, binomial(2*k, k)*binomial(n, 4*k));
\\ Partial sums are A393245.
for(n=0, 40, print1(sum(k=0, n , a(k))-a393245(n),", "));