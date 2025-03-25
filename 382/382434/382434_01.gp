M=21;

\\ a(n) = Sum_{k=0..n} A080233(n,k)^4 = Sum_{k=0..n} A156644(n,k)^4.
\\ a(n) = Sum_{k=0..n} ( binomial(n,k) - binomial(n,k-1) )^4.
a(n) = sum(k=0, n, (binomial(n,k) - binomial(n,k-1))^4);
for(n=0, M, print1(a(n), ", "))
