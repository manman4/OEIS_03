\\ A(n,k) = Sum_{j=0..n} 2^j * binomial(n,j) * binomial(k*n+1,j).
a(n, k) = sum(j=0, n, 2^j * binomial(n, j) * binomial(k*n+1, j));
for(n=0, 9, for(k=0, n, print1(a(k,n-k),", ")));


