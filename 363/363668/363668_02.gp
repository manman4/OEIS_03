a(n) = polcoef(sum(k=1, n, 1/(1 - k*x^k)^n + x*O(x^n) - 1), n);
for(n=1, 26, print1(a(n),", "))  

