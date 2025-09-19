\\ a(n) = [x^n] ((1+x)^3 + x + x^2)^n.
a(n) = polcoef( ( (1+x)^3 + x + x^2 + x*O(x^n) )^n, n);
for(n=0, 21, print1(a(n), ", ")); 


