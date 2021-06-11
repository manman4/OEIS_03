M=26;

a(n) = sum(k=1, n, sumdiv(k, d, eulerphi(k/d)*binomial(d+n-2, n-1)));
for(n=0, M, print1(a(n), ", "));