M=23;

\\ a(n) = Sum_{k=0..n} binomial(n,k) * |Stirling1(k,3)|.
a(n) = sum(k=0, n, binomial(n,k) * abs(stirling(k,3,1)));
for(n=0, M, print1(a(n),", "))       