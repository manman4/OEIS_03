\\ a(n) = Sum_{k=0..n} (2*k+1) * binomial(2*k,k) * binomial(k,n-k). 
a(n) = sum(k=0, n, (2*k+1)*binomial(2*k, k)*binomial(k, n-k));
for(n=0, 23, print1(a(n), ", ")) 