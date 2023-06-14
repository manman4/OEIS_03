a(n) = polcoef(sum(k=1, n, binomial(k+n,n) * x^k/(1 - x^k) + x*O(x^n)), n);
for(n=1, 25, print1(a(n),", "))  

