M=22;

a(n) = sum(k=0, n, 3^(n-k)*binomial(n-1, n-k)*binomial(2*k, k));
for(n=0, M, print1(a(n), ", "));  

\\ Cf. A122898
for(n=0, M, print1(a(n)/2, ", ")); 

b(n) = sum(i=0, n, sum(j=0,i, (1/3)^i*a(j)*a(i-j)));
for(n=0, M, print1(b(n), ", "));  
