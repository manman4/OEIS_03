a(n) = sumdiv(n, d, moebius(n/d) * (n/d)^3 * sigma(d,5));
for(n=1, 33, print1(a(n), ", "))
