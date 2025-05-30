M=10;

\\ A(n,k) = Product_{j=1..n} (1 - (-k)^j)/(1 + k).
a(n,k) = prod(j=1, n, (1-(-k)^j)/(1+k));
for(n=0, M, for(k=0, n, print1(a(k, n-k),", ")));


