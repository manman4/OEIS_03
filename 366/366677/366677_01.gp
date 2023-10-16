a(n) = sum(k=0, n\4, binomial(3*(n-4*k)+1,k) * binomial(4*(n-4*k),n-4*k)/(3*(n-4*k)+1) );                    
for(n=0, 21, print1(a(n),", "))  

