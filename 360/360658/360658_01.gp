a(n) = if(n==1, 1, -sum(k=2, n, k^3*a(n\k))); 
for(n=1, 42, print1(a(n),", "))
