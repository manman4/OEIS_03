a(n) = sumdiv(n, d, moebius(n/d) * n/d * sigma(d,4));
for(n=1, 36, print1(a(n), ", "))
