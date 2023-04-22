T(n, k) = if(k==0, 1, n! * polcoef(( -2 * lambertw(-k*x^3/2 * exp(3*x))/(k*x^3) )^(1/3), n));
for(n=0, 10, for(k=0, n , print1(T(k,n-k), ", ")))
