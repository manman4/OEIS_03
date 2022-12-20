M=24;

a(n) = sumdiv(n, d, n/d*4^(n-d));
for(n=1, M, print1(a(n), ", "));