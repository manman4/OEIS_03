\\ a(n) = Sum_{k=0..floor(n/3)} binomial(2*k,k) * binomial(n-1,n-3*k).
a(n) = sum(k=0, n\3, binomial(2*k, k)*binomial(n-1, n-3*k));
for(n=0, 32, print1(a(n),", "));

\\ a(n) = Sum_{k=0..floor(n/3)} binomial(2*k,k) * binomial(n,3*k).
a393244(n) = sum(k=0, n\3, binomial(2*k, k)*binomial(n, 3*k));
\\ Partial sums are A393244.
for(n=0, 40, print1(sum(k=0, n , a(k))-a393244(n),", "));