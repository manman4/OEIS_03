M=21;

\\ a(n) = Sum_{k=0..n} binomial(4*k+1,k) * binomial(k+1,n-k)/(4*k+1).
a(n) = sum(k=0, n, binomial(4*k+1, k) * binomial(k+1, n-k) / (4*k+1));
for(n=0, M, print1(a(n), ", "));

\\ a(n) = A365178(n) + A365178(n-1).
a365178(n) = sum(k=0, n, binomial(k, n-k)*binomial(4*k, k)/(3*k+1));
b(n) = a365178(n) + a365178(n-1);
for(n=0, 100, print1(a(n)-b(n), ", "));


 