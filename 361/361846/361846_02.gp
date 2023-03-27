a(n) = polcoef(1/(1 - 9*x*(1+x)^n + x*O(x^n))^(1/3), n);
for(n=0, 17, print1(a(n),", "))    