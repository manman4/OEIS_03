\\ a(0) = 1, a(1) = 2; a(n) = (1/(n-1)) * Sum_{k=0..floor(n/2)} 2^(n-2*k) * binomial(2*n-2-k,n-2) * binomial(n-k,k).
a(n) = if(n<2, n+1, (1/(n-1)) * sum(k=0, n\2, 2^(n-2*k) * binomial(2*n-2-k,n-2) * binomial(n-k,k)));
for(n=0, 22, print1(a(n),", "));