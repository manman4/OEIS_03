M=23;

\\ a(n) = Sum_{k=0..n-1} binomial(n,k) * |Stirling1(k,3)|.
a(n) = sum(k=0, n-1, binomial(n,k) * abs(stirling(k,3,1)));
for(n=0, M, print1(a(n),", "))

a000399(n) = abs(stirling(n, 3, 1));
for(n=3, M, print1(a000399(n),", "))
a381022(n) = sum(k=0, n, binomial(n, k)*abs(stirling(k, 3, 1)));
\\ a(n) = A381022(n) - A000399(n).
b(n) = a381022(n) - a000399(n);
for(n=0, 100, print1(a(n)-b(n),", "))       