\\ a(n) = Sum_{k=0..2*n} (-2*n)^k * binomial(6*n,k) * Stirling2(6*n-k,4*n).
a(n) = sum(k=0, 2*n, (-2*n)^k * binomial(6*n,k) * stirling(6*n-k, 4*n, 2));
for(n=0, 20, print1(a(n),", "));
