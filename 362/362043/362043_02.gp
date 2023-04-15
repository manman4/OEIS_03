M=11;

T(n, k) = n! * polcoef(exp(x + k*x^3/6 + x*O(x^n)), n);
for(n=0, M, for(k=0, n, print1(T(k,n-k), ", "))); 