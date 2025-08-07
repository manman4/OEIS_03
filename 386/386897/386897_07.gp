M=17;

\\ a(n) = [x^n] 1/(1-4*x)^(3*n/2+1).
a(n) = polcoef( 1/(1-4*x + x*O(x^n))^(3*n/2+1), n);
for(n=0, M, print1(a(n)", "));

\\ a(n) = [x^n] (1+4*x)^(5*n/2).
b(n) = polcoef( (1+4*x + x*O(x^n))^(5*n/2), n);
for(n=0, 100, print1(a(n)-b(n)", "));