\\ a(n) = Sum_{k=0..n} (3-i)^k * (3+i)^(n-k) * binomial(n,k)^2, where i is the imaginary unit.
a(n) = sum(k=0, n, (3-I)^k * (3+I)^(n-k) * binomial(n,k)^2);
for(n=0, 23, print1(a(n),", "));

