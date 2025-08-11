M=25;

\\ a(n) = [x^n] 1/((1-9*x)^(1/3) * (1-x)^n).
a(n) = polcoef( 1/( (1-9*x +x*O(x^n))^(1/3) * (1-x)^n ), n);
for(n=0, M, print1(a(n)", "));

