M=24;

a(n) = sumdiv(n, d, d*4^(d-1));
for(n=1, M, print1(a(n), ", "));