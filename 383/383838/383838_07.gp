\\ a(n) = Sum_{k=0..2*n} (-4)^k * binomial(2*n+8,k) * Stirling2(2*n-k+8,8).
a(n) = sum(k=0, 2*n, (-4)^k * binomial(2*n+8,k) * stirling(2*n-k+8, 8, 2));
for(n=0, 20, print1(a(n),", "));

