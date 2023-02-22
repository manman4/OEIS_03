M=18;

a(n) = sumdiv(n, d, d^(d+n/d-1)*binomial(d+n/d-1, d));
for(n=1, M, print1(a(n), ", "));
for(n=1, M, if(a(n)==n+n^n, print1(n, ", ")));
