M=43;

a(n) = sumdiv(n, d, d*(n/d)^d);
for(n=1, M, print1(a(n), ", "));