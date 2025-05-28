\\ A(n,k) = (1/3) * Sum_{j>=0} (2/3)^j * binomial(j,n)^k.
a(n, k) = (1/3) * sum(j=0, 1000, (2/3)^j * binomial(j, n)^k)+ 0.;
for(n=0, 8, for(k=0, n, print1(a(k,n-k), ", ")));
matrix(4, 4, n, k, a(n-1, k-1))
a(n, k) = round((1/3) * sum(j=0, 1000, (2/3)^j * binomial(j, n)^k)+ 0.);
for(n=0, 8, for(k=0, n, print1(a(k,n-k), ", ")));

