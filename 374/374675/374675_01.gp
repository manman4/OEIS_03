a(n) = sum(k=0, n, binomial(n+k-1, n)*binomial(n+k, n));
for(n=0, 19, print1(a(n),", "))     
for(n=0, 19, print1(a(n)/(n+1),", "))               
