M=43;

a(n) = sumdiv(n, d, sigma(d)^(n/d));
for(n=1, M, print1(a(n), ", "));