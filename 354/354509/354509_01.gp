M=23;

a(n) = n!*sum(k=1, n, sumdiv(k, d, (-1)^(d+1)/(d*(k/d)!))/(n-k)!);
for(n=1, M, print1(a(n), ", "));