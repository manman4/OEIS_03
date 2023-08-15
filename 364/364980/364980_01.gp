a(n) = n!*sum(k=0, n, k^(n-k) * binomial(2*n-k+1,k)/( (2*n-k+1)*(n-k)! ) );                        
for(n=0, 17, print1(a(n),", "))                                             
