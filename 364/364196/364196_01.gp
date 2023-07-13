a(n) = sum(k=0, n, binomial(n, k)*binomial(5*n+3*k+1, n)/(5*n+3*k+1));
for(n=0, 18, print1(a(n),", "))                                                   
 