a128899(n, k) = binomial(2*n-2, n-k)-binomial(2*n-2, n-k-2);
for(n=0, 10, for(k=0, n, print1(a128899(n, k), ", ")));

\\ Let b_k(n) = Sum_{j=0..n} A128899(n,j)^k. b_1(n) = binomial(2*n-1,n) = A088218(n) and b_2(n) = A024492(n-1) for n > 0.
b(n, k) = sum(j=0, n, a128899(n, j)^k);
for(n=0, 17, print1(b(n, 1), ", "))
for(n=0, 17, print1(b(n, 2), ", "))


\\ a(n) = Sum_{k=0..n} A128899(n,k)^3.
a(n) = sum(k=0, n, a128899(n, k)^3);
for(n=0, 17, print1(a(n), ", "))

