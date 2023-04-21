T(n, k) = if(k==0, 1, n! * polcoef(-2 * lambertw(-k*x^2/2 * exp(x + x*O(x^(n+5)) ) )/(k*x^2), n));
for(n=0, 10, for(k=0, n , print1(T(k,n-k), ", ")))
