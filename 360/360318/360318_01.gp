M=22;

a(n) = sum(k=0, n, 3^(n-k)*binomial(n-1, n-k)*binomial(2*k, k));
for(n=0, M, print1(a(n), ", "));  

\\ Cf. A122898
for(n=0, M, print1(a(n)/2, ", ")); 
