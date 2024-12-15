M=25;

\\ a(n) = [x^n] 1/( 1/(1 + x) - x^3 )^(3*n).
a(n) = polcoef( 1/( 1/(1+x) - x^3 + x*O(x^n))^(3*n), n);
for(n=0, M, print1(a(n),", "))