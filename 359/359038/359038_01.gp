M=71;

a(n) = sumdiv(n, d, numdiv(d^7));
for(n=1, M, print1(a(n), ", "));