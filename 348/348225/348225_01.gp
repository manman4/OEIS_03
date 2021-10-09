M=20;

a(n) = sumdiv(n, d, binomial(n, d)^d);
for(n=1, M, print1(a(n), ", "));