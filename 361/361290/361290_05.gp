M=10;

T(n, k) = if(k==0, n==1, round(n!*polcoef( exp(k * x +x*O(x^n) ) * sinh(sqrt(k) * x +x*O(x^n) ) / sqrt(k), n)));
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));


