a(n) = sumdiv(n, d, moebius(n/d) * (n/d)^4 * sigma(d, 9));
for(n=1, 23, print1(a(n), ", "))
