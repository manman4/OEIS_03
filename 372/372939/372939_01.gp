a(n) = sumdiv(n, d, moebius(n/d)*d^n*numdiv(d));
for(n=1, 18, print1(a(n), ", "));

