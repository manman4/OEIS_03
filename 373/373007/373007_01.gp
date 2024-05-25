a(n) = sumdiv(n, d, moebius(n/d) * (n/d)^2 * sigma(d, 7));
for(n=1, 27, print1(a(n), ", "))
