M=9;

T(n, k) = n! * polcoef(exp(-x) / (1 + lambertw(-k*x) + x*O(x^n)), n);
for(n=0, M, for(k=0, n, print1(T(k,n-k),", "))) 
