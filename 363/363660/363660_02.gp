a(n) = polcoef(sum(k=1, n, (1/(1 - x^k)^(n+1) + x*O(x^n)) - 1), n);
for(n=1, 25, print1(a(n),", "))  

