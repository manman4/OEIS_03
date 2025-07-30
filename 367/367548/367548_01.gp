\\ a(n) = [x^n] 1/((1-2*x) * (1+x)^n).
a(n) = polcoef(1/( (1-2*x) * (1+x)^n + x*O(x^n) ), n);
for(n=0, 26, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n} (-1)^k * 3^(n-k) * binomial(2*n,k).
b(n) = sum(k=0, n, (-1)^k * 3^(n-k) * binomial(2*n, k));
for(n=0, 50, print1(a(n)-b(n), ", "));