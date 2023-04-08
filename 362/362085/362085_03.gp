a(n) = polcoef(1/(1-1*x*(1+x)^n + x*O(x^n))^3, n);

for(n=0, 20, print1(a(n),", "))  