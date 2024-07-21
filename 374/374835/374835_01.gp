\\ a(n) = Sum_{k=0..n-1} (-1)^k * (k+1)*(k+2)/(2*n-k) * binomial(3*n-2-2*k,n-1-k).
a(n) = sum(k=0, n-1, (-1)^k * (k+1)*(k+2)/(2*n-k) * binomial(3*n-2-2*k,n-1-k));
for(n=0, 24, print1(a(n), ", "))