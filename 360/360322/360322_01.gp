M=25;

a(n) = sum(k=0, n, (-5)^(n-k)*binomial(n-1, n-k)*binomial(2*k, k));
for(n=0, M, print1(a(n), ", "));  

\\ Cf. A007317
for(n=0, M, print1((-1)^(n+1) * a(n)/2, ", ")); 
