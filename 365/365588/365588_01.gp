a(n) = sum(k=0, n, 5^k*k!*abs(stirling(n, k, 1)));
for(n=0, 16, print1(a(n),", "))  

