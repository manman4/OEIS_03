\\ a(n) = Sum_{k=0..floor(n/2)} (6*k+1) * binomial(4*n-2*k+1,n-2*k)/(4*n-2*k+1).
a(n) = sum(k=0, n\2, (6*k+1)*binomial(4*n-2*k+1, n-2*k)/(4*n-2*k+1));
for(n=0, 22, print1(a(n), ", "));

\\ a(n) = (1/(3*n+1)) * Sum_{k=0..floor(n/2)} (6*k+1) * binomial(4*n-2*k,n-2*k).
b(n) = sum(k=0, n\2, (6*k+1)*binomial(4*n-2*k, n-2*k))/(3*n+1);
for(n=0, 100, print1(a(n)-b(n), ", "));

