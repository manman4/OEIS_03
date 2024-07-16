a(n) = sum(k=0, n, binomial(n+k-1, n)*binomial(n+k, n)^2);
for(n=0, 16, print1(a(n),", "))     
          
