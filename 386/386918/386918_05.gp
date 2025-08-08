M=25;

\\ a(n) = [x^n] (1+2*x)^(4*n).
a(n) = polcoef( (1+2*x +x*O(x^n))^(4*n), n);
for(n=0, M, print1(a(n)", "));

