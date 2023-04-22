T(n, k) = if(k==0, 1, n! * polcoef(sqrt( -lambertw(-k*x^2 * exp(2*x))/(k*x^2) ), n));
for(n=0, 10, for(k=0, n , print1(T(k,n-k), ", ")))
