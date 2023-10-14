M=51;

a(n) = n*sumdiv(n, d, sigma(d)^2/d);
for(n=1, M, print1(a(n), ", "));