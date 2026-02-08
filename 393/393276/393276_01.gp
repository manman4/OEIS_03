\\ a(n) = Sum_{k=0..floor(n/4)} binomial(2*k,k) * binomial(n-k-1,n-4*k).
a(n) = sum(k=0, n\4, binomial(2*k, k)*binomial(n-k-1, n-4*k));
for(n=0, 36, print1(a(n),", "));

\\ a(n) = Sum_{k=0..floor(n/4)} binomial(2*k,k) * binomial(n-k,3*k).
a393246(n) = sum(k=0, n\4, binomial(2*k, k)*binomial(n-k, 3*k));
\\ Partial sums are A393246.
for(n=0, 40, print1(sum(k=0, n , a(k))-a393246(n),", "));