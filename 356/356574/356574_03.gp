M=76;

a(n) = sumdiv(n, d, numdiv(n^2));
for(n=1, M, print1(a(n), ", "));