\\ a(n) = Sum_{d|n} phi(n/d) * (n/d)^2 * sigma_2(d^2)/sigma(d^2).
a(n) = sumdiv(n, d, eulerphi(n/d) * (n/d)^2 * sigma(d^2, 2)/sigma(d^2, 1));

for(n=1, 50, print1(a(n), ", "))

\\ a(n) = Sum_{1 <= x_1, x_2 <= n} ( gcd(x_1, n)/gcd(x_1, x_2, n) )^2. 
b(n) = sum(i=1, n, sum(j=1, n, ( gcd(i, n)/gcd([i, j, n]) )^2 ));
for(n=1, 30, print1(a(n)-b(n), ", "))
