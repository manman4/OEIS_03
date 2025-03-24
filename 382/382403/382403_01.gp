a039599(n, k) = (2*k+1)/(n+k+1)*binomial(2*n, n-k);
for(n=0, 9, for(k=0, n, print1(a039599(n, k), ", ")));

\\ Let b_k(n) = Sum_{j=0..n} A039599(n,j)^k. b_1(n) = binomial(2*n,n) = A000984(n) and b_2(n) = binomial(4*n,2*n)/(2*n+1) = A048990(n).
b(n, k) = sum(j=0, n, a039599(n, j)^k);
for(n=0, 17, print1(b(n, 1), ", "))
for(n=0, 17, print1(b(n, 2), ", "))


\\ a(n) = Sum_{k=0..n} A039599(n,k)^3.
a(n) = sum(k=0, n, a039599(n, k)^3);
for(n=0, 17, print1(a(n), ", "))

