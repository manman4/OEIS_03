a(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, ( n/gcd([i, j, k, n]) )^2 )));
for(n=1, 30, print1(a(n), ", "))

\\ a(n) = Sum_{1 <= x_1, x_2, x_3 <= n} ( gcd(x_1, n)/gcd(x_1, x_2, x_3, n) )^3.
b(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, ( gcd([i, n])/gcd([i, j, k, n]) )^3 )));
for(n=1, 30, print1(a(n)-b(n), ", "))

