M=25;

\\ a(n) = [x^n] ( (1 + x)/(1 - x - x^2) )^(4*n).
a(n) = polcoef( ( (1+x)/(1-x-x^2) + x*O(x^n) )^(4*n), n);
for(n=0, M, print1(a(n),", "))