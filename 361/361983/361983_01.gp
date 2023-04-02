a(n) = 1 + sum(k=2, n, (-1)^k*k^2*a(n\k));    
for(n=1, 50, print1(a(n),", "))
