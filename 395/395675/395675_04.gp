\\ a(n) = ((n+1)/2) * Sum_{k=0..n+1} binomial(2*n+1,k).
a(n) = ((n+1)/2) * sum(k=0, n+1, binomial(2*n+1,k));
for(n=0, 30, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n} binomial(k+2,2) * binomial(2*n+2,n-k). 
b(n) = sum(k=0, n, binomial(k+2,2) * binomial(2*n+2,n-k));
for(n=0, 50, print1(a(n)-b(n), ", "));
