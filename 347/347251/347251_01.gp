M=20;

a(n) = sumdiv(n, d, moebius(d)*moebius(n/d)*d^n);
for(n=1, M, print1(a(n), ", "));