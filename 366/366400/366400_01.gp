b(n) = sum(k=0, n, binomial(n+3*k/2,n-k) * binomial(5*k/2,k) / (3*k/2+1) );                    
for(n=0, 22, print1(b(n),", "))  

a(n, r=2, s=-1, t=4, u=2) = r*sum(k=0, n, binomial(t*k+u*(n-k)+r, k)*binomial(s*k, n-k)/(t*k+u*(n-k)+r)); 
for(n=0, 100, print1(a(n)-b(n),", "))