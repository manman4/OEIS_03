M=22;

\\ a(n) = Sum_{k=0..n} (-1)^(n-k) * binomial(n,k) * |Stirling1(k+1,3)|.
a(n) = sum(k=0, n, (-1)^(n-k) * binomial(n,k) * abs(stirling(k+1,3,1)));
for(n=0, M, print1(a(n),", "))

\\ a(n) = Sum_{k=0..n} (-1)^(n-k) * binomial(n,k) * |Stirling1(k,3)|.
a381065(n) = sum(k=0, n, (-1)^(n-k) * binomial(n,k) * abs(stirling(k,3,1)));
\\ a(n) = A381065(n) + A381065(n+1).
b(n) = a381065(n) + a381065(n+1);
for(n=0, 100, print1(a(n)-b(n),", "))       