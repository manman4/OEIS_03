\\ b(n) = Sum_{k=0..n} binomial(n+k, k)^2.
b(n) = sum(k=0, n, binomial(n+k, k)^2);

\\ a(n) = binomial(2*n,n) * (4 * binomial(2*n,n)^2 - 3 * A112029(n)).
a(n) = binomial(2*n,n) * (4 * binomial(2*n,n)^2 - 3 * b(n));
for(n=0, 17, print1(a(n), ", "))

