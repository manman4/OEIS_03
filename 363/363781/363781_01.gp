a(n) = polcoef(1/sum(k=0, n^(1/3), x^k^3)^n +x*O(x^n), n);
for(n=0, 25, print1(a(n),", ")) 
