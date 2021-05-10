M=47;

a(n) = sumdiv(n, i, sumdiv(n, j, sumdiv(n, k, i*j*k/gcd(gcd(i, j), k))));
for(n=1, M, print1(a(n), ", "));