a(n) = (n!/(2*n+1)) * sum(k=0, n, k^(n-k) * binomial(2*n+1,k)/(n-k)! );                        
for(n=0, 17, print1(a(n),", "))                                             
