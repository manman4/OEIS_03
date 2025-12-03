\\ a(n) = Sum_{k=0..floor(n/2)} (4*k+2) * binomial(3*n-2*k+2,n-2*k)/(3*n-2*k+2).
a(n) = sum(k=0, n\2, (4*k+2)*binomial(3*n-2*k+2, n-2*k)/(3*n-2*k+2));
for(n=0, 23, print1(a(n), ", "));

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/2)} (2*k+1) * binomial(3*n-2*k+1,n-2*k).
b(n) = sum(k=0, n\2, (2*k+1)*binomial(3*n-2*k+1, n-2*k))/(n+1);
for(n=0, 100, print1(a(n)-b(n), ", "));

