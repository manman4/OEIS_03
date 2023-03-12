M=18;

a(n) = if(n==0, 0, round(n!*polcoef( exp(n * x +x*O(x^n) ) * sinh(sqrt(n) * x +x*O(x^n) ) / sqrt(n), n)));
for(n=0, M, print1(a(n), ", "));


