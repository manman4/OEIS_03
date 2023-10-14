a(n) = n! * sum(k=0, n, k^(n-k) * binomial(4*k+1,k)/( (4*k+1)*(n-k)! ) );                        
for(n=0, 16, print1(a(n),", "))        

a(n) = n!*sum(k=0, n, k^(n-k)*binomial(4*k, k)/((3*k+1)*(n-k)!));                   
for(n=0, 16, print1(a(n),", "))      
