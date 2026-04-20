a000182(k) = 2^(2*k)*(2^(2*k)-1)*abs(bernfrac(2*k))/(2*k);
for(n=1, 16, print1(a000182(n), ", "));

\\ a(n) = (1/16) * Sum_{k=0..n} (10 + 5*9^(n-k) + 25^(n-k)) * binomial(2*n,2*k) * A000182(k+1).
a(n) = 1/16 * sum(k=0, n, (10 + 5*9^(n-k) + 25^(n-k)) * binomial(2*n, 2*k) * a000182(k+1));
for(n=0, 15, print1(a(n),", "));

