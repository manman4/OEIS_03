M=12;

a(n) = n!*4^(n*(n-1)/2)*polcoef(exp(sum(k=1, n, x^k/(k!*4^(k*(k-1)/2)))+x*O(x^n)), n);
for(n=0, M, print1(a(n), ", "));