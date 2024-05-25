a(n) = sumdiv(n, d, moebius(n/d) * n/d * sigma(d, 6));
for(n=1, 30, print1(a(n), ", "))
