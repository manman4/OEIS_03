a(n) = if(n==0, 1, sum(k=0, n, (k/(n+k)) * binomial(n+k,k)^2 ));
for(n=0, 19, print1(a(n),", "))     
         
