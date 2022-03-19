M=20;

a(n) = if(n==0, 1, -sum(k=1, n, issquare(k)*a(n-k))); 
for(n=0, M, print1(a(n), ", "));