a(n) = 4^n * sum(k=0, n+1000, (1/2.0)^(k+1) * binomial(n-1+3*k/2, n));                                                                                     
for(n=0, 25, print1(round(a(n)),", "))   
for(n=0, 25, print1(a(n),", ")) 

