\\ a(n) = Sum_{j=0..n} binomial(2*j,j)^4 * binomial(n,j)^2 * Sum_{k=0..n} binomial(k+j,k-j)^2 * binomial(2*j,n-k) * binomial(2*j,k-j).
a(n) = sum(j=0, n, binomial(2*j,j)^4 * binomial(n,j)^2 * sum(k=0, n, binomial(k+j,k-j)^2 * binomial(2*j,n-k) * binomial(2*j,k-j)));
for(n=0, 13, print1(a(n),", "))

\\ A092815(n) = Sum_{k=0..n} binomial(n,k) * binomial(n+k,k) * a(k).
b(n) = sum(k=0, n, binomial(n,k) * binomial(n+k,k) * a(k));
a092815(n) = sum(k=0, n, binomial(n,k)^5 * binomial(n+k,k)^5 );
for(n=0, 100, print1(b(n)-a092815(n),", "))
