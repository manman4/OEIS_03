M=26;

\\ a(n) = Sum_{k=0..3} (-1)^k * 2^(3-k) * binomial(3,k) * k^n.
a(n) = sum(k=0, 3, (-1)^k * 2^(3-k) * binomial(3,k) * k^n);
for(n=0, M, print1(a(n), ", ")) 