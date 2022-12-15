M=8;

a(n) = sumdiv(n, d, sigma(d, d)^d);
for(n=1, M, print1(a(n), ", "));