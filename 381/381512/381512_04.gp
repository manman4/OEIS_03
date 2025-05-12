\\ A(n,k) = (1/(2^k*k!)) * Sum_{j=0..k} (-1)^j * (k-2*j)^(2*n+k) * binomial(k,j).
a(n, k) = (1/(2^k*k!)) * sum(j=0, k, (-1)^j * (k-2*j)^(2*n+k) * binomial(k,j));
for(n=0, 10, for(k=0, n, print1(a(k, n-k), ", ")));
