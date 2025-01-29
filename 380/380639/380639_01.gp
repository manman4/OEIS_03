M=18;

\\ a(n) = n! * Sum_{k=0..n} 2^k * binomial(2*n-k-1,k)/(n-k)!.
a(n) = n!*sum(k=0, n, 2^k*binomial(2*n-k-1, k)/(n-k)!);
for(n=0, M, print1(a(n), ", "));