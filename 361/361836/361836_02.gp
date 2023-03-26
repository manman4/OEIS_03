a(n) = polcoef(1/(1 - x*(1-x)^n + x*O(x^n)), n);
for(n=0, 23, print1(a(n),", ")) 

