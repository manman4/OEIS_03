M=18;

a(n) = sumdiv(n, d, (n/d)^(n/d)*binomial(d+n-2, n-1));
for(n=1, M, print1(a(n), ", "));