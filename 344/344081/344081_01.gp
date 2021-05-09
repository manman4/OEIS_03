M=24;

a(n) = sumdiv(n, d, numdiv(d)^d);
for(n=1, M, print1(a(n), ", "));