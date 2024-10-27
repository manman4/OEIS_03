M=22;

\\ a(n) = Sum_{k=0..3} k! * binomial(3,k) * Stirling1(n,k).
a(n) = sum(k=0, 3, k! * binomial(3,k) * stirling(n,k,1));
for(n=0, M, print1(a(n), ", ")) 