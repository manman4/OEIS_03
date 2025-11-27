\\ a(n) = Sum_{k=0..floor(n/3)} (6*k+1) * binomial(3*n-3*k+1,n-3*k)/(3*n-3*k+1).
a(n) = sum(k=0, n\3, (6*k+1)*binomial(3*n-3*k+1, n-3*k)/(3*n-3*k+1));
for(n=0, 24, print1(a(n), ", "));

\\ a(n) = (1/(2*n+1)) * Sum_{k=0..floor(n/3)} (6*k+1) * binomial(3*n-3*k,n-3*k).
b(n) = sum(k=0, n\3, (6*k+1)*binomial(3*n-3*k, n-3*k))/(2*n+1);
for(n=0, 100, print1(a(n)-b(n), ", "));