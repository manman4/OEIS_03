\\ a(n) = ((n+1)/10) * A079678(n+1) 
a079678(n) = sum(k=0, n, 4^(n-k)*binomial(5*n+1, k));
for(n=0, 30, print1(a079678(n), ", "));
a(n) = ((n+1)/10) * a079678(n+1);
for(n=0, 30, print1(a(n), ", "));

\\ a(n) = ((n+1)/10) * Sum_{k=0..n+1} 4^(n+1-k) * binomial(5*n+6,k).
b(n) = ((n+1)/10) * sum(k=0, n+1, 4^(n+1-k) * binomial(5*n+6,k));
for(n=0, 30, print1(a(n)-b(n), ", "));

\\ a(n) = Sum_{k=0..n} 4^k * binomial(k+2,2) * binomial(5*n+6,n-k).
c(n) = sum(k=0, n, 4^k * binomial(k+2,2) * binomial(5*n+6,n-k));
for(n=0, 50, print1(a(n)-c(n), ", "));

