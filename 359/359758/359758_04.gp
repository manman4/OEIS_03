a(n) = if(n==0, 1, (2/n) * sum(k=0, n-1, (n+k) * binomial(n+3-k,4) * a(k)));
for(n=0, 20, print1(a(n),", "))   
