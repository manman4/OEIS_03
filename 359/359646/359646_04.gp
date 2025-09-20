\\ a(n) = [x^n] ( (1+2*x) * (1+x)^5 )^n. 
a(n) = polcoef( ( (1+2*x) * (1+x)^5 + x*O(x^n) )^n, n); 
for(n=0, 19, print1(a(n), ", "));


