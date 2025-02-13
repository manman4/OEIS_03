M=22;

\\ a(n) = Sum_{k=0..n} (-1)^(n-k) * binomial(n,k) * |Stirling1(k+1,4)|.
a(n) = sum(k=0, n, (-1)^(n-k) * binomial(n,k) * abs(stirling(k+1,4,1)));
for(n=0, M, print1(a(n),", "))

\\ a(n) = Sum_{k=0..n} (-1)^(n-k) * binomial(n,k) * |Stirling1(k,4)|.
a381066(n) = sum(k=0, n, (-1)^(n-k) * binomial(n,k) * abs(stirling(k,4,1)));
\\ a(n) = A381066(n) + A381066(n+1).
b(n) = a381066(n) + a381066(n+1);
for(n=0, 100, print1(a(n)-b(n),", "))       