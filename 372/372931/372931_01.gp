a(n) = sumdiv(n, d, moebius(n/d)*d^4*numdiv(d));
for(n=1, 35, print1(a(n), ", "))
