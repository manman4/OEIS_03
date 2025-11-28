\\ a(n) = Sum_{k=0..floor(n/2)} (6*k+2) * binomial(4*n-2*k+2,n-2*k)/(4*n-2*k+2).
a(n) = sum(k=0, n\2, (6*k+2)*binomial(4*n-2*k+2, n-2*k)/(4*n-2*k+2));
for(n=0, 21, print1(a(n), ", "));

\\ a(n) = (1/(3*n+2)) * Sum_{k=0..floor(n/2)} (6*k+2) * binomial(4*n-2*k+1,n-2*k).
b(n) = sum(k=0, n\2, (6*k+2)*binomial(4*n-2*k+1, n-2*k))/(3*n+2);
for(n=0, 100, print1(a(n)-b(n), ", "));

