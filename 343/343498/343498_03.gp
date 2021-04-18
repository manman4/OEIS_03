M=37;

a(n) = sumdiv(n, d, moebius(n/d)*d*sigma(d, 3));
for(n=1, M, print1(a(n),", ")); 