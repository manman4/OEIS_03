\\ a(0) = 1; a(n) = Sum_{k=0..floor((n-1)/2)} 2^k * (2*k+1) * binomial(n-1,2*k) * a(n-2*k-1).
a(n) = if(n==0, 1, sum(k=0, (n-1)\2, 2^k * (2*k+1) * binomial(n-1,2*k) * a(n-2*k-1) )); 
for(n=0, 20, print1(a(n), ", "))
