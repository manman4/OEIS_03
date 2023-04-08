a(n) = polcoef(1/(1-1*x*(1+x)^n + x*O(x^n))^2, n);

for(n=0, 21, print1(a(n),", "))  