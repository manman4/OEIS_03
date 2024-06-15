T(n, k) = sum(j=0, k*n\(2*k+1), binomial(k*(n-2*j), j));
for(n=0, 12, for(k=0, n, print1(T(k, n-k),", "))) 
