M=20;

a(n) = n!*sum(k=1, n, sumdiv(k, d, d^(k/d+1))/k);
for(n=1, M, print1(a(n), ", "));