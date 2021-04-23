M=19;

a(n) = sumdiv(n, d, (n/d)^n*binomial(d+n/d-1, d));
for(n=1, M, print1(a(n), ", "));