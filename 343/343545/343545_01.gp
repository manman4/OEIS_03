M=44;

a(n) = n*sumdiv(n, d, binomial(d+3, 4)/d);
for(n=1, M, print1(a(n),", "));