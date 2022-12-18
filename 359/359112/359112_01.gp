M=29;

a(n) = sumdiv(n, d, n/d*d^(n-d));
for(n=1, M, print1(a(n), ", "));