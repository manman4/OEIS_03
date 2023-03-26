a(n) = if(n==0, 1, sum(k=0, n-1, k^k * a(n-1-k)));         
for(n=0, 19, print1(a(n),", ")) 