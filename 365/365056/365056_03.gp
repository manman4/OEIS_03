a(n) = n!*sum(k=0, n, (1/2)^(n-k) * (-k+1)^(k-1) * binomial(k,n-k)/k! );                                
for(n=0, 20, print1(a(n),", "))   

