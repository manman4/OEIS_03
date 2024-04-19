\\ a(n) = Sum_{k=0..n} 9^k * binomial(n/3+2*k/3-2/3,k) * binomial(n-1,n-k)/(n-k+1).
a(n) = sum(k=0, n, 9^k * binomial(n/3+2*k/3-2/3,k) * binomial(n-1,n-k)/(n-k+1));
for(n=0, 20, print1(a(n), ", "))

