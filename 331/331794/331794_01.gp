\\ a(n) = Sum_{k=0..n} n^k * binomial(n+1,k) * binomial(n+1,k+1).
a(n) = sum(k=0, n, n^k*binomial(n+1, k)*binomial(n+1, k+1));
for(n=0, 20, print1(a(n),", "))  
