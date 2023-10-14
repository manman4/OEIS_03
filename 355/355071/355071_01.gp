M=11;

a(n) = n!*4^(n*(n-1)/2)*polcoef(log(sum(k=0, n, x^k/(k!*4^(k*(k-1)/2)))+x*O(x^n)), n);
for(n=0, M, print1(a(n), ", "));