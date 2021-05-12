M=54;

a(n) = sumdiv(n, i, sumdiv(n, j, sumdiv(n, k, n/gcd([i, j, k]))));
for(n=1, M, print1(a(n), ", "));