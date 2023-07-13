a(n) = sum(k=0, n, binomial(n, k)*binomial(2*n+3*k+1, n)/(2*n+3*k+1));
for(n=0, 19, print1(a(n),", "))                                                   
 