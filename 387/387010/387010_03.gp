M=25;

\\ a(n) = [x^n] 1/((1-x)^(3*n+3) * (1-2*x)).
a(n) = polcoef( 1/( (1-x)^(3*n+3) * (1-2*x) +x*O(x^n) ), n);
for(n=0, M, print1(a(n)", "));

