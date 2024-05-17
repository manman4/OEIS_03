a(n) = sumdiv(n, d, moebius(n/d)*d^3*sigma(d, 2));
for(n=1, 32, print1(a(n), ", "))
