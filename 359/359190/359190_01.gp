M=26;

a(n) = sumdiv(n, d, d*4^(n/d-1));
for(n=1, M, print1(a(n), ", "));