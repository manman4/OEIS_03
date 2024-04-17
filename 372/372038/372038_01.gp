\\ a(n) = Sum_{k=0..n} 9^k * binomial(k/3+1/3,k) * binomial(k,n-k)/(k+1).
a(n) =sum(k=0, n, 9^k * binomial(k/3+1/3,k) * binomial(k,n-k)/(k+1));
for(n=0, 26, print1(a(n), ", "))

