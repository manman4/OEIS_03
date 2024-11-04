M=22;

\\ a(n) = n! * Sum_{k=0..floor(n/2)} k! * binomial(2,k) * |Stirling1(n-k,k)|/(n-k)!.
a(n) = n! * sum(k=0, n\2, k! * binomial(2,k) * abs(stirling(n-k,k,1))/(n-k)!);
for(n=0, M, print1(a(n), ", ")) 