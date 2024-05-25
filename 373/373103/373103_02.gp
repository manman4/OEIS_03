a(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, sum(l=1, n, sum(m=1, n, ( n/gcd([i, j, k, l, m, n]) )^4)))));
for(n=1, 20, print1(a(n), ", "))

b(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, sum(l=1, n, sum(m=1, n, ( gcd(i, n)/gcd([i, j, k, l, m, n]) )^5)))));
for(n=1, 20, print1(a(n)-b(n), ", "))

c(n) = sumdiv(n, d, eulerphi(n/d) * (n/d)^8 * sigma(d^2, 8)/sigma(d^2, 4));
for(n=1, 30, print1(a(n)-c(n), ", "))
