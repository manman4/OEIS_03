a(n) = if(n==1, 1, -sum(k=2, n, k^2*a(n\k))); 
for(n=1, 52, print1(a(n),", "))
