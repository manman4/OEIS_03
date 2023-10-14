M=49;

a(n) = n*sumdiv(n, d, binomial(d+2, 3)/d);
for(n=1, M, print1(a(n),", "));