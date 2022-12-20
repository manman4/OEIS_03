M=25;

a(n) = sumdiv(n, d, 4^(n-d));
for(n=1, M, print1(a(n), ", "));