M=21;

a(n) = n!*sum(k=1, n, sumdiv(k, d, (-1)^(k/d+1)*d^2)/(k*(n-k)!));
for(n=1, M, print1(a(n), ", "));
