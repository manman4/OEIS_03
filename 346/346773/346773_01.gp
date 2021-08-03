M=103;

a(n) = sumdiv(n, d, moebius(d)^n);
for(n=1, M, print1(a(n), ", "));