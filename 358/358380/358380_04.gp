M=76;

a(n) = sumdiv(n, d, numdiv(n^3/d));
for(n=1, M, print1(a(n), ", "));