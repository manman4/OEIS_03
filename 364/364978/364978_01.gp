a(n) = n!*sum(k=0, n, k^(n-k) * binomial(2*n-2*k+1,k)/( (2*n-2*k+1)*(n-k)! ) );                        
for(n=0, 18, print1(a(n),", "))                                             
