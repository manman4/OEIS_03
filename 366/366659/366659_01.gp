a(n) = sum(k=1, n, (-1)^(k-1) * binomial(floor(n/k)+3,4) );                    
for(n=1, 43, print1(a(n),", "))                                                 
