T(n, k) = if(k==0, 1, n! * polcoef(-6 * lambertw(-k*x^3/6 * exp(x + x*O(x^(n+5)) ) )/(k*x^3), n));
for(n=0, 10, for(k=0, n , print1(T(k,n-k), ", ")))
