M=73;

a(n) = sumdiv(n, d, numdiv(n*d^4));
for(n=1, M, print1(a(n), ", "));