M=73;

a(n) = sumdiv(n, d, 6^omega(d));
for(n=1, M, print1(a(n), ", "));