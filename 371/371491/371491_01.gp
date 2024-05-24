a(n) = sumdiv(n, d, moebius(n/d) * (n/d)^3 * sigma(d, 8));
for(n=1, 24, print1(a(n), ", "))



