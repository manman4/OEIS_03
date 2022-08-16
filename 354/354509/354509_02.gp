M=23;

a352013(n) = sumdiv(n, d, (-1)^(n/d+1)*(n-1)!/(d-1)!);
a(n) = sum(k=1, n, a352013(k)*binomial(n, k));
for(n=1, M, print1(a(n), ", "));