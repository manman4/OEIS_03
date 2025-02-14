M=22;

\\ a(n) = Sum_{k=0..n-1} binomial(n,k) * |Stirling1(k+1,3)|.
a(n) = sum(k=0, n-1, binomial(n,k) * abs(stirling(k+1,3,1)));
for(n=0, M, print1(a(n),", "))

a000399(n) = abs(stirling(n, 3, 1));
for(n=0, M, print1(a000399(n),", "))
a381024(n) = sum(k=0, n, binomial(n, k)*abs(stirling(k+1, 3, 1)));
\\ a(n) = A381024(n) - A000399(n+1).
b(n) = a381024(n) - a000399(n+1);
for(n=0, 100, print1(a(n)-b(n),", "))       