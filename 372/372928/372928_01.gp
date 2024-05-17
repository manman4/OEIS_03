a(n) = sumdiv(n, d, moebius(n/d)*d^3*numdiv(d));
for(n=1, 40, print1(a(n), ", "))
