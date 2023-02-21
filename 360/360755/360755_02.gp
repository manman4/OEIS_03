M=34;

a(n) = sumdiv(n, d, 2^(d-1)*binomial(d, n/d-1));
for(n=1, M, print1(a(n), ", "));
for(n=1, M, print1(a(n)-2^(n-1), ", "));