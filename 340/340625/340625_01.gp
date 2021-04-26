M=74;

a(n) = sumdiv(n, d, (d%2)*binomial(n/d, d));
for(n=1, M, print1(a(n),", "));