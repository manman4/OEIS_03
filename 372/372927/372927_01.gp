a(n) = sumdiv(n, d, moebius(n/d)*d^2*sigma(d, 3));
for(n=1, 33, print1(a(n), ", "))
