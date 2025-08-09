\\ a(n) = [x^n] 1/((1-x)^(2*n+1) * (1-2*x)^n).
a(n) = polcoef( 1/( (1-x)^(2*n+1) * (1-2*x)^n +x*O(x^n) ), n);
for(n=0, 20, print1(a(n), ", "));



