M=22;

\\ a(n) = Sum_{k=0..4} k! * binomial(4,k) * Stirling1(n,k).
a(n) = sum(k=0, 4, k! * binomial(4,k) * stirling(n,k,1));
for(n=0, M, print1(a(n), ", ")) 