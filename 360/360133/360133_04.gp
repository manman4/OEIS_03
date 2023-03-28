a(n) = if(n==0, 1, (2/n) * sum(k=0, n-1, (-1)^(n-1-k) * (n+k) * binomial(n+1-k,2) * a(k)));
for(n=0, 20, print1(a(n),", "))   
