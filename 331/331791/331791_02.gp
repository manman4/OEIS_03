\\ T(n,k) = [x^n] (1+k*x+(k-1)*x^2)^(n+1).
T(n,k) = polcoeff( (1+k*x+(k-1)*x^2)^(n+1), n);
for(n=0, 10, for(k=0, n, print1(T(k,n-k),", ")));