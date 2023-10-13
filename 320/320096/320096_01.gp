M=15;

a(n) = sum(k=0, n, (-1)^(n-k)*k!*k^n*stirling(n, k, 1)); 
for(n=0, M, print1(a(n), ", "));