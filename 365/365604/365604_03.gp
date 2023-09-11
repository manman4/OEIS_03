a(n) = if(n==0, 1, 5 * sum(k=1, n, (-1)^(k-1) * (k-1)! * binomial(n,k) * a(n-k) )); 
for(n=0, 16, print1(a(n),", "))  

