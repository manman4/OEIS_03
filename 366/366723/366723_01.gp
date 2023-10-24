a(n) = sum(k=1, n, (-1)^(k-1) * binomial(floor(n/k)+4,5) );                    
for(n=1, 40, print1(a(n),", "))                                                 
