T(n, k) = n! * polcoef(exp(x - lambertw(-k*x^3/6 * exp(x + x*O(x^(n+5))))), n);
for(n=0, 10, for(k=0, n , print1(T(k,n-k), ", ")))
