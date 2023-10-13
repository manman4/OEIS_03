M=30;

a(n) = sumdiv(n, d, d^(n/d+n));
for(n=1, M, print1(a(n), ", "));