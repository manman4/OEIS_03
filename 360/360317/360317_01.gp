M=23;

a(n) = sum(k=0, n, 2^(n-k)*binomial(n-1, n-k)*binomial(2*k, k));
for(n=0, M, print1(a(n), ", "));  

\\ Cf. A005573
for(n=0, M, print1(a(n)/2, ", ")); 

b(n) = sum(i=0, n, sum(j=0,i, (1/2)^i*a(j)*a(i-j)));
for(n=0, M, print1(b(n), ", "));  
