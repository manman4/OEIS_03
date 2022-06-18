M=13;

a(n) = n!*3^(n*(n-1)/2)*polcoef(exp(sum(k=1, n, x^k/(k!*3^(k*(k-1)/2)))+x*O(x^n)), n);
for(n=0, M, print1(a(n), ", "));