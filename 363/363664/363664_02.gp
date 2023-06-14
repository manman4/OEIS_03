a(n) = polcoef(sum(k=1, n, x^k/(1 - (k*x)^k)^(n+1) + x*O(x^n)), n);
for(n=1, 24, print1(a(n),", "))  

