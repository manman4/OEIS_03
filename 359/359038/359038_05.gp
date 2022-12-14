M=71;

a(n) = sumdiv(n, d, numdiv(n^4/d));
for(n=1, M, print1(a(n), ", "));