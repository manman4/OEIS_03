\\ a(n) = Sum_{k=0..n} 4^k * binomial(1/2,k) * binomial(n-1,n-k).
a(n) = sum(k=0, n, 4^k * binomial(n-1,n-k) * binomial(1/2,k));
for(n=0, 30, print1(a(n), ", "))

\\ a(n) = (-1)^(n-1) * 2 * A005043(n-1) for n > 0.
a005043(n) = if( n<0, 0, n++; polcoef( serreverse( (x - x^3) / (1 + x^3) + x * O(x^n)), n));
for(n=1, 100, print1(a(n) - (-1)^(n-1) * 2 * a005043(n-1), ", "))
