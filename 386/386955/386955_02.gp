M=25;

\\ a(n) = [x^n] 1/((1-3*x)^2 * (1-x)^n).
a(n) = polcoef( 1/( (1-3*x +x*O(x^n))^2 * (1-x)^n ), n);
for(n=0, M, print1(a(n)", "));

