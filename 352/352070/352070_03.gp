a(n) = if(n==0, 1, n!* sum(k=1, n, (-3)^k * (2/(3*n) - 1/k) * a(n-k)/(n-k)!));
for(n=0, 20, print1(a(n),", "))        
 