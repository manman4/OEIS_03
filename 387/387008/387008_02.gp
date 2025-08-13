M=25;

\\ a(n) = [x^n] (1+x)^(3*n+3)/(1-x).
a(n) = polcoef( (1+x)^(3*n+3)/(1-x + x*O(x^n)), n);
for(n=0, M, print1(a(n)", "));

