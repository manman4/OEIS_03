\\ a(n) = Sum_{k=0..2*n} (-5)^k * binomial(2*n+10,k) * Stirling2(2*n-k+10,10).
a(n) = sum(k=0, 2*n, (-5)^k * binomial(2*n+10,k) * stirling(2*n-k+10, 10, 2));
for(n=0, 16, print1(a(n),", "));

