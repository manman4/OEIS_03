M=25;

\\ a(n) = [x^n] 1/((1-9*x) * (1-x)^(n+1)).
a(n) = polcoef( 1/( (1-9*x +x*O(x^n)) * (1-x)^(n+1) ), n);
for(n=0, M, print1(a(n)", "));

