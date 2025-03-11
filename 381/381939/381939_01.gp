M=21;

\\ a(n) = Sum_{k=0..n} binomial(4*k+1,k) * binomial(3*k+3,n-k)/(4*k+1).
a(n) = sum(k=0, n, binomial(4*k+1, k) * binomial(3*k+3, n-k) / (4*k+1));
for(n=0, M, print1(a(n), ", "));

\\ a(n) = A367641(n) + A367641(n-1).
a367641(n) = sum(k=0, n, binomial(3*k+2, n-k)*binomial(4*k, k)/(3*k+1));
b(n) = a367641(n) + a367641(n-1);
for(n=0, 100, print1(a(n)-b(n), ", "));

