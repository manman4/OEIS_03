\\ a(n) = Sum_{k=0..n} (n-i)^k * (n+i)^(n-k) * binomial(n,k)^2, where i is the imaginary unit.
a(n) = sum(k=0, n, (n-I)^k * (n+I)^(n-k) * binomial(n,k)^2);
for(n=0, 23, print1(a(n),", "));

