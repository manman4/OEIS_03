M=25;

\\ a(n) = [x^n] (1+x)^(4*n)/(1-x)^(2*n+1).
a(n) = polcoef( (1+x)^(4*n)/(1-x + x*O(x^n))^(2*n+1), n);
for(n=0, M, print1(a(n)", "));

