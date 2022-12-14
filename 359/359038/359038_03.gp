M=71;

a(n) = sumdiv(n, d, numdiv(n^2*d^3));
for(n=1, M, print1(a(n), ", "));