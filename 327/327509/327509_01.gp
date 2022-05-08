M=30;

a(n) = sum(k=0, n\8, (8*k)!*stirling(n, 8*k, 2)/(8!^k*k!)); 
for(n=0, M, print1(a(n), ", "));