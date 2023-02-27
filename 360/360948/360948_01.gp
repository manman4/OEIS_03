M=43;

a(n) = sumdiv(n, d, (n/d)^(d-1)*binomial(d+n/d-1, d));
for(n=1, M, print1(a(n), ", "));
for(n=1, M, if(a(n)==1+n, print1(n, ", ")));
