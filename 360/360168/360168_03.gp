\\ a(n) = [x^n] 1/(((1-x)^3-x^3) * (1-x)^(n-2)). 
a(n) = polcoef(1/(((1-x)^3-x^3) * (1-x)^(n-2) +x*O(x^n)), n);
for(n=0, 20, print1(a(n),", "))


