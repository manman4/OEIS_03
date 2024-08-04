a(n) = 9^n * sum(k=0, n+1000, (1/2.0)^(k+1) * binomial(2*k/3, n));                                                                                     
for(n=0, 25, print1(round(a(n)),", "))   
for(n=0, 25, print1(a(n),", ")) 

