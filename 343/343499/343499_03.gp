M=34;

a(n) = sumdiv(n, d, moebius(n/d)*d*sigma(d, 4));
for(n=1, M, print1(a(n),", ")); 