M=27;

a(n) = n*sumdiv(n, d, binomial(d+n-1, n)/d);
for(n=1, M, print1(a(n),", "));