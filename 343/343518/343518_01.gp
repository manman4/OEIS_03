M=40;

a(n) = sum(a=1, n, sum(b=1, a, sum(c=1, b, sum(d=1, c, gcd(gcd(gcd(gcd(n, a), b), c), d)))));
for(n=1, M, print1(a(n),", "));  