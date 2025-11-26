\\ a(n) = Sum_{k=0..floor(n/4)} (4*k+1) * binomial(2*n-4*k+1,n-4*k)/(2*n-4*k+1).
a(n) = sum(k=0, n\4, (4*k+1)*binomial(2*n-4*k+1, n-4*k)/(2*n-4*k+1));
for(n=0, 28, print1(a(n), ", "));

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/4)} (4*k+1) * binomial(2*n-4*k,n-4*k).
b(n) = sum(k=0, n\4, (4*k+1)*binomial(2*n-4*k, n-4*k))/(n+1);
for(n=0, 100, print1(a(n)-b(n), ", "));