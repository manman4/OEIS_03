\\ A(n,k) = Sum_{i=0..k*n} 2^i * Sum_{j=0..i} (-1)^j * binomial(i,j) * binomial(i-j,n)^k.
a(n, k) = sum(i=0, k*n, 2^i*sum(j=0, i, (-1)^j*binomial(i, j)*binomial(i-j, n)^k));
for(n=0, 8, for(k=0, n, print1(a(k,n-k), ", ")));