M=20;

J(n, k) = sumdiv(n, d, moebius(d) * (n / d)^k);
\\ a(n) = Sum_{d|n} phi(n/d) * (n/d) * sigma_2(d^2)/sigma(d^2).
a(n) = sumdiv(n, d, J(n/d, 1) * (n/d) * sigma(d^2, 2) / sigma(d^2));
for(n=1, 30, print1(a(n),", "));

b(n) = sum(i=1, n, sum(j=1, n, gcd(i, n)/gcd([i, j, n])));
for(n=1, 30, print1(a(n)-b(n),", "));


a(n) = sumdiv(n, d, J(n/d, 2) * (n/d) * sigma(d^2, 2) / sigma(d^2));
for(n=1, 30, print1(a(n),", "));

b(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, gcd([i, j, n])/gcd([i, j, k, n]))));
for(n=1, 30, print1(a(n)-b(n),", "));


a(n) = sumdiv(n, d, J(n/d, 3) * (n/d) * sigma(d^2, 2) / sigma(d^2));
for(n=1, 30, print1(a(n),", "));

b(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, sum(l=1, n, gcd([i, j, k, n])/gcd([i, j, k, l, n])))));
for(n=1, 20, print1(a(n)-b(n),", "));



a(n) = sumdiv(n, d, J(n/d, 4) * (n/d) * sigma(d^2, 2) / sigma(d^2));
for(n=1, 30, print1(a(n),", "));

b(n) = sum(i=1, n, sum(j=1, n, sum(k=1, n, sum(l=1, n, sum(m=1, n, gcd([i, j, k, l, n])/gcd([i, j, k, l, m, n]))))));
for(n=1, 20, print1(a(n)-b(n),", "));