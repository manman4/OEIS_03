a039599(n, k) = (2*k+1)/(n+k+1)*binomial(2*n, n-k);
for(n=0, 9, for(k=0, n, print1(a039599(n, k), ", ")));

\\ a(n) = -Sum_{k=0..n} (-1)^k * A039599(n,k)^3.
a(n) = -sum(k=0, n, (-1)^k * a039599(n, k)^3);
for(n=0, 17, print1(a(n), ", "))

