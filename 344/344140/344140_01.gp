M=26;

a(n) = sumdiv(n, i, sumdiv(n, j, sumdiv(n, k,sumdiv(n, l,sumdiv(n, m,sumdiv(n, a,sumdiv(n, b, sumdiv(n, c,   gcd(gcd(gcd(gcd(gcd(gcd(gcd(i, j), k), l), m), a), b), c))))))))); 
b(n) = sumdiv(n, i, sumdiv(n, j, sumdiv(n, k,sumdiv(n, l,sumdiv(n, m,sumdiv(n, a,sumdiv(n, b, sumdiv(n, c, n/lcm(lcm(lcm(lcm(lcm(lcm(lcm(i, j), k), l), m), a), b), c)))))))));
c(n) = sumdiv(n, d, eulerphi(n/d)*numdiv(d)^8);

for(n=1, M, print1(a(n)-c(n), ", "));
for(n=1, M, print1(b(n)-c(n), ", "));

d(n) = sumdiv(n, d, eulerphi(n/d)*numdiv(d)^n);
for(n=1, M, print1(d(n), ", "));