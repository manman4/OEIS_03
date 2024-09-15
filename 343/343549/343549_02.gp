M=27;

a(n) = sumdiv(n, d, binomial(d+n-1, d));
for(n=1, M, print1(a(n),", "));
