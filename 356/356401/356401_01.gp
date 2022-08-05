M=22;

a(n) = n!*sum(k=1, n, sumdiv(k, d, (-1)^(d+1)/(d*(k/d)!)));
for(n=1, M, print1(a(n), ", "));