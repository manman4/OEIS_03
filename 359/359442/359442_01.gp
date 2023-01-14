M=32;

a(n) = sumdiv(n, d, d^(n+1-d-n/d));
for(n=1, M, print1(a(n), ", "));