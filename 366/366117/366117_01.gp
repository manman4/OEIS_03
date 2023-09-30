a(n) = (1/(n+1)) * sum(k=0, n\3, (-1)^k * binomial(n+k,k) * binomial(2*n-k+2,n-3*k) );
for(n=0, 27, print1(a(n),", "))   

