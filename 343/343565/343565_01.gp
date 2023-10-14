M=27;

a(n) = sumdiv(n, d, moebius(n/d)*binomial(d+n-1, n));
for(n=1, M, print1(a(n), ", "));