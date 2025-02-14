M=22;

\\ a(n) = Sum_{k=0..n-1} binomial(n,k) * |Stirling1(k+1,2)|.
a(n) = sum(k=0, n-1, binomial(n,k) * abs(stirling(k+1,2,1)));
for(n=0, M, print1(a(n),", "))

a000254(n) = abs(stirling(n+1, 2, 1));
for(n=0, M, print1(a000254(n),", "))
a073596(n) = sum(k=0, n, binomial(n, k)*abs(stirling(k+1, 2, 1)));
\\ a(n) = A073596(n) - A000254(n).
b(n) = a073596(n) - a000254(n);
for(n=0, 100, print1(a(n)-b(n),", "))       