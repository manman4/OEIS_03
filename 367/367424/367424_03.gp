a(n) = if(n==0, 1, sum(k=1, n, 3^k * (1 - 2/3 * k/n) * (k-1)! * binomial(n,k) * a(n-k)));      
for(n=0, 17, print1(a(n),", "))   
