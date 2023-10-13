M=20;

a(n) = sumdiv(n, d, (n/d)^(n-d));
for(n=1, M, print1(a(n), ", "));