a(n) = sumdiv(n, d, moebius(n/d)*d^3*sigma(d));
for(n=1, 36, print1(a(n), ", "))
