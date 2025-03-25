M=21;

\\ a(n) = Sum_{k=0..n} binomial(n,k) * ( binomial(n,k) - binomial(n,k-1) )^5.
a382433(n) = sum(k=0, n, binomial(n, k) * (binomial(n, k) - binomial(n, k-1))^5);
for(n=0, M, print1(a382433(n), ", "))

\\ a(n) = 2 * A382433(n) - 1.
a(n) = 2 * a382433(n) - 1;
for(n=0, M, print1(a(n), ", "))

