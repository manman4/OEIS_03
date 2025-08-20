\\ A(n,k) = Sum_{j=0..n} k^j * binomial(n,j) * Catalan(j+1).
catalan(n) = binomial(2*n, n)/(n+1);
a(n, k) = sum(j=0, n, k^j * binomial(n, j) * catalan(j+1));
for(n=0, 10, for(k=0, n, print1(a(k, n-k),", ")));
