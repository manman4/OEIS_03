M=18;

\\ A(n,k) = (n+1)! * Sum_{j=0..n} (-1)^j * binomial(-k*n,j)/(n+1-j).
a(n, k) = (n+1)! * sum(j=0, n, (-1)^j * binomial(-k*n, j)/(n+1-j));
for(n=0, M, for(k=0, n, print1(a(k, n-k),", ")));


