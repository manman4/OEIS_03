\\ A(n,k) = (2*n)! * Sum_{j=0..n} 16^j * (-3)^(n-j) * binomial(n+j+k/2-1,n+j) * binomial(n+j,2*j).
a(n, k) = (2*n)! * sum(j=0, n, 16^j * (-3)^(n-j) * binomial(n+j+k/2-1, n+j) * binomial(n+j, 2*j));
\\ matrix(7, 7, n, k, a(n-1, k-1))

for(n=0, 9, for(k=0, n, print1(a(k, n-k),", ")));