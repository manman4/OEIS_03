M=39;

a(n) = n*sumdiv(n, d, binomial(d+4, 5)/d);
for(n=1, M, print1(a(n),", "));