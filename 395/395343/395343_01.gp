a000182(k) = 2^(2*k)*(2^(2*k)-1)*abs(bernfrac(2*k))/(2*k);
for(n=1, 16, print1(a000182(n), ", "));

\\ a(n) = (1/(2*4^n)) * Sum_{k=0..n} (0^(n-k) + 4^(n-k)) * binomial(2*n,2*k) * A000182(k+1).
a(n) = 1/(2*4^n) * sum(k=0, n, (0^(n-k) + 4^(n-k)) * binomial(2*n, 2*k) * a000182(k+1));
for(n=0, 19, print1(a(n),", "));

