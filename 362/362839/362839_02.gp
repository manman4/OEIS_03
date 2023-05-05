M=10;

T(n, k) = n!* polcoef(exp(x * (exp(k * x) - 1)), n);
for(n=0, M, for(k=0, n, print1(T(k,n-k),", "))) 
