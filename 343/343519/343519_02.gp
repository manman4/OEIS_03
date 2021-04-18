M=40;

a(n) = sumdiv(n, d, eulerphi(n/d)*binomial(d+4, 5));
for(n=1, M, print1(a(n),", "));  