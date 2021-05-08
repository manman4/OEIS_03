M=44;

a(n) = n*sumdiv(n, d, sigma(d)^3/d);
for(n=1, M, print1(a(n), ", "));