M=61;

a(n) = n*sumdiv(n, d, numdiv(d)^3/d);
for(n=1, M, print1(a(n), ", "));