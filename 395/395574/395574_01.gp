\\ a(0) = 1; a(n) = Sum_{k=1..floor((n+1)/2)} (-1)^(k-1) * (2*n-2*k+1)^k * binomial(n-k+1,k) * a(n-k).
a(n) = if(n==0, 1, sum(k=1, (n+1)\2, (-1)^(k-1) * (2*n-2*k+1)^k * binomial(n-k+1,k) * a(n-k)));
for(n=0, 15, print1(a(n),", "));