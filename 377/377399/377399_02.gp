M=25;

\\ a(n) = Sum_{k=0..4} (-1)^k * 2^(4-k) * binomial(4,k) * k^n.
a(n) = sum(k=0, 4, (-1)^k * 2^(4-k) * binomial(4,k) * k^n);
for(n=0, M, print1(a(n), ", ")) 