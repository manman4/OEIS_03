a(n) = if(n==0, 1, sum(k=1, n, (-4)^k * (1/4 * k/n - 1) * (k-1)! * binomial(n,k) * a(n-k)));      
for(n=0, 18, print1(a(n),", "))   
