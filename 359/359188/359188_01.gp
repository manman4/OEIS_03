M=43;

a(n) = sumdiv(n, d, moebius(n/d)*d*(n/d)^(d-1));
for(n=1, M, print1(a(n), ", "));