M=9;

T(n, k) = n! * polcoef(exp(-k*x) / (1 + lambertw(-x) + x*O(x^n)), n);
for(n=0, M, for(k=0, n, print1(T(k,n-k),", "))) 
