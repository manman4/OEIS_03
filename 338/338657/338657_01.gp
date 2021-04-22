M=55;

a(n) = sumdiv(n, d, moebius(d)*binomial(d+n/d-1, d));
for(n=1, M, print1(a(n), ", "));