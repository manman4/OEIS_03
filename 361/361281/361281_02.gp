a(n) = n!*polcoef(exp(x * (1+x)^n + x*O(x^n)), n);
for(n=0, 17, print1(a(n),", ")) 