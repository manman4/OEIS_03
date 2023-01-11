M=19;

a(n) = sumdiv(n, d, d^(d+n/d-2));
for(n=1, M, print1(a(n), ", "));