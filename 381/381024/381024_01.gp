M=22;

\\ a(n) = Sum_{k=0..n} binomial(n,k) * |Stirling1(k+1,3)|.
a(n) = sum(k=0, n, binomial(n,k) * abs(stirling(k+1,3,1)));
for(n=0, M, print1(a(n),", "))

\\ a(n) = Sum_{k=0..n} binomial(n,k) * |Stirling1(k,3)|.
a381022(n) = sum(k=0, n, binomial(n,k) * abs(stirling(k,3,1)));
\\ a(n) = A381022(n+1) - A381022(n).
b(n) = a381022(n+1) - a381022(n);
for(n=0, 100, print1(a(n)-b(n),", "))       