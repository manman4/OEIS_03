M=76;

a(n) = sumdiv(n, d, moebius(d)^2*numdiv(d)^3);
for(n=1, M, print1(a(n), ", "));