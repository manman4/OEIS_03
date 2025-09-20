\\ a(n) = [x^n] ( (1+2*x) * (1+x)^n )^n.
a(n) = polcoef( ( (1+2*x) * (1+x)^n + x*O(x^n) )^n, n);
for(n=0, 19, print1(a(n), ", "));


