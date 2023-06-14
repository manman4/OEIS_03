a(n) = polcoef(sum(k=1, n, x^k/(1 - k*x^k)^n + x*O(x^n)), n);
for(n=1, 26, print1(a(n),", "))  

