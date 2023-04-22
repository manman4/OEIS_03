T(n, k) = n! * polcoef(exp(x - lambertw(-k*x^3/2 * exp(3*x))/3), n);
for(n=0, 10, for(k=0, n , print1(T(k,n-k), ", ")))
