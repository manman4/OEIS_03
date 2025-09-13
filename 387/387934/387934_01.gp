\\ A(n,k) = Sum_{j=0..n} binomial(n,j) * binomial(k*n+j,j).
a(n, k) = sum(j=0, n, binomial(n, j) * binomial(k*n + j, j));
for(n=0, 9, for(k=0, n, print1(a(k,n-k),", ")));


