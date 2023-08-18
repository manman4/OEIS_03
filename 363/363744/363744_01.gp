a(n) = n!*sum(k=0, n, (n-k+1)^(k-1) * binomial(2*k,n-k)/k! );                                
for(n=0, 18, print1(a(n),", "))   

