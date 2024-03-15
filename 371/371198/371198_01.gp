M=20;

\\ a(n) = n! * Sum_{j=0..n} Sum_{k=0..floor(j/3)} k! * binomial(j-2*k,n-j-k) * |Stirling1(j-2*k,k)|/(j-2*k)!.
a(n) = n! * sum(j=0, n, sum(k=0, j\3, k! * binomial(j-2*k,n-j-k) * abs(stirling(j-2*k,k,1))/(j-2*k)!));
for(n=0, M, print1(a(n)", "))