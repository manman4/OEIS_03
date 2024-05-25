a(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, sum(l=1, n, sum(m=1, n, ( n/gcd([i, j, k, l, m, n]) )^5)))));
for(n=1, 20, print1(a(n), ", "))

b(n) = sumdiv(n, d, moebius(n/d) * (n/d)^5 * sigma(d, 10));
for(n=1, 30, print1(a(n)-b(n), ", "))
