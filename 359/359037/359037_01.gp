M=73;

a(n) = sumdiv(n, d, numdiv(d^6));
for(n=1, M, print1(a(n), ", "));