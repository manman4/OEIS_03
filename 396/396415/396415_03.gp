\\ a(n) = Sum_{k=0..n} (k+1) * binomial(3*k+1,k) * binomial(3*n-2*k+1,n-k)/((3*k+1) * (3*n-2*k+1)).
a396414(n) = sum(k=0, n, (k+1) * binomial(3*k+1,k) * binomial(3*n-2*k+1,n-k)/((3*k+1) * (3*n-2*k+1)));
for(n=0, 22, print1(a396414(n),", "));

\\ a(n) = Sum_{k=0..n} (k+1) * binomial(3*n-2*k+1,n-k)/(3*n-2*k+1) * A396414(k).
a396415(n) = sum(k=0, n, (k+1) * binomial(3*n-2*k+1,n-k)/(3*n-2*k+1) * a396414(k));
for(n=0, 22, print1(a396415(n),", "));
