M=18;

a(n) = sumdiv(n, d, (n/d)^(n/d)*binomial(d+n-1, n));
for(n=1, M, print1(a(n), ", "));