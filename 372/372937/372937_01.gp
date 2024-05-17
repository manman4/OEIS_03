a(n) = sumdiv(n, d, moebius(n/d)*d^4*sigma(d));
for(n=1, 32, print1(a(n), ", "))
