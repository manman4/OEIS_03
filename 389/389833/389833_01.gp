a(n) = n!*polcoef(exp(x*(1+n*exp(x+x*O(x^n)))), n); 
for(n=0, 20, print1(a(n),", "))  