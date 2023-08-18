a(n) = if(n==0, 1, n!*sum(k=1, n, (-n+k+1)^(k-1) * binomial(k,n-k)/k! ));                                
for(n=0, 20, print1(a(n),", "))   

