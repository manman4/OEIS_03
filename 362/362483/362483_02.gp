T(n, k) = n! * polcoef(exp(x - lambertw(-k*x^2 * exp(2*x))/2), n);
for(n=0, 10, for(k=0, n , print1(T(k,n-k), ", ")))
