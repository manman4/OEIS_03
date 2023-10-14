M=40;

a(n) = sum(a=1, n, sum(b=1, a, sum(c=1, b, sum(d=1, c, sum(e=1, d, gcd(gcd(gcd(gcd(gcd(n, a), b), c), d), e))))));
for(n=1, M, print1(a(n),", "));  