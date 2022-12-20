M=27;

a(n) = sumdiv(n, d, n/d*3^(n-d));
for(n=1, M, print1(a(n), ", "));