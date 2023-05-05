M=10;

T(n, k) = n!* polcoef(exp(k * x * (exp(x) - 1)), n);
for(n=0, M, for(k=0, n, print1(T(k,n-k),", "))) 
