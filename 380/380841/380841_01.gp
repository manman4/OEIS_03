M=10;

\\ A(n,k) = n! * Sum_{j=0..n} j^(n-j) * binomial(j+k-1,j)/(n-j)!.
a(n,k) = n! * sum(j=0, n, j^(n-j) * binomial(j+k-1, j)/(n-j)!);
for(n=0, M, for(k=0, n, print1(a(n-k, k),", ")));

