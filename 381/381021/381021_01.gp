M=22;

\\ a(n) = Sum_{k=0..n} binomial(n,k) * |Stirling1(k,2)|.
a(n) = sum(k=0, n, binomial(n,k) * abs(stirling(k,2,1)));
for(n=0, M, print1(a(n),", "))       