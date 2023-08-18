a(n) = n!*sum(k=0, n, (-2*n+2*k+1)^(k-1) * binomial(k,n-k)/k! );                                
for(n=0, 19, print1(a(n),", "))   

