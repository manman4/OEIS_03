a(n) = sumdiv(n, d, moebius(n/d)*d^2*sigma(d, 2));
for(n=1, 36, print1(a(n), ", "))
