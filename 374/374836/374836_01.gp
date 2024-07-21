\\ a(n) = Sum_{k=0..n-2} (-1)^k * (k+1)*(k+2)*(k+3)/(2*(2*n-1-k)) * binomial(3*n-4-2*k,n-2-k).
a(n) = sum(k=0, n-2, (-1)^k * (k+1)*(k+2)*(k+3)/(2*(2*n-1-k)) * binomial(3*n-4-2*k,n-2-k));
for(n=0, 25, print1(a(n), ", "))