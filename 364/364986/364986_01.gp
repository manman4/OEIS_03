a(n) = (n!/(3*n+1)) * sum(k=0, n, k^(n-k) * binomial(3*n+1,k)/(n-k)! );                        
for(n=0, 16, print1(a(n),", "))                                             
