a000364(n) = if(n<0, 0, (2*n)! * polcoeff( 1 / cos(x + O(x^(2*n + 1))), 2*n));
for(n=0, 16, print1(a000364(n), ", "));

\\ a(n) = (1/16) * Sum_{k=0..n} 2^k * (-1)^(n-k) * (10 + 5*9^(n-k) + 25^(n-k)) * binomial(2*n,2*k) * A000364(k).
a(n) = 1/16 * sum(k=0, n, 2^k * (-1)^(n-k) * (10 + 5*9^(n-k) + 25^(n-k)) * binomial(2*n, 2*k) * a000364(k));
for(n=0, 16, print1(a(n),", "));

