a(n) = if(n==0, 1, sum(k=1, n, (-1)^k * (k/n - 3) * binomial(n,k) * a(n-k) ));
for(n=0, 18, print1(a(n),", "))    
