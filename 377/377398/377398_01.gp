M=26;

\\ a(n) = Sum_{k=0..3} (-1)^k * k! * binomial(3,k) * Stirling2(n,k).
a(n) = sum(k=0, 3, (-1)^k * k! * binomial(3,k) * stirling(n,k,2));
for(n=0, M, print1(a(n), ", ")) 