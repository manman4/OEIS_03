M=10;

T(n, k) = n!* polcoef(1/(1 - k * x)^x, n);
for(n=0, M, for(k=0, n, print1(T(k,n-k),", "))) 
