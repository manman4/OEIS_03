M=40;

a(n) = sumdiv(n, d, eulerphi(n/d)*binomial(d+6, 7));
for(n=1, M, print1(a(n),", "));