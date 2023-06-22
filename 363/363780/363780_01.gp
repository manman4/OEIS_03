a(n) = polcoef(1/sum(k=0, n^(1/2), x^k^2)^n +x*O(x^n), n);
for(n=0, 26, print1(a(n),", ")) 
