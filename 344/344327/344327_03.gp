M=82;

a(n) = sumdiv(n, d, 4^omega(d));
for(n=1, M, print1(a(n), ", "));