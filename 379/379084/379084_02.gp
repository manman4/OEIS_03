M=25;

\\ a(n) = [x^n] 1/( 1/(1 + x) - x^2 )^(2*n).
a(n) = polcoef( 1/( 1/(1+x) - x^2 + x*O(x^n))^(2*n), n);
for(n=0, M, print1(a(n),", "))