M=71;

a(n) = sumdiv(n, d, numdiv(n*d^5));
for(n=1, M, print1(a(n), ", "));