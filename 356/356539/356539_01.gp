M=28;

a(n) = sumdiv(n, d, d*3^(n-d));
for(n=1, M, print1(a(n), ", "));