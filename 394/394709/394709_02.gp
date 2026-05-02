\\ Coefficient of x^(2*n) in the expansion of ( (1+x)^3 * (1+x^2) )^n.
a(n) = polcoef( ( (1+x)^3 * (1+x^2) )^n, 2*n);  
for(n=0, 25, print1(a(n),", "));
