\\ a(n) = (1/(n+1)) * [x^n] (1+x^4+x^5)^(n+1).
a(n) = polcoef( ( 1 + x^4 + x^5 + x*O(x^n) )^(n+1), n)/(n+1);
for(n=0, 35, print1(a(n), ", ")); 


