M=27;

\\ A332508
a332508(n) = sumdiv(n, d, binomial(d+n-2, n-1));
for(n=1, M, print1(a332508(n),", "));

a(n) = n*sumdiv(n, d, binomial(d+n-2, n-1)/d);
for(n=1, M, print1(a(n),", "));