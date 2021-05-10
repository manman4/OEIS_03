M=53;

a(n) = sumdiv(n, i, sumdiv(n, j, sumdiv(n, k, sumdiv(n, l, sumdiv(n, m, gcd(gcd(gcd(gcd(i, j), k), l), m))))));
for(n=1, M, print1(a(n), ", "));