M=23;

\\ a(n) = Sum_{k=0..n-1} binomial(n,k) * |Stirling1(k,2)|.
a(n) = sum(k=0, n-1, binomial(n,k) * abs(stirling(k,2,1)));   
for(n=0, M, print1(a(n),", "))

a000254(n) = abs(stirling(n+1, 2, 1));
for(n=0, M, print1(a000254(n),", "))
a381021(n) = sum(k=0, n, binomial(n, k)*abs(stirling(k, 2, 1)));
\\ a(n) = A381021(n) - A000254(n-1).
b(n) = a381021(n) - a000254(n-1);
for(n=0, 100, print1(a(n)-b(n),", "))       