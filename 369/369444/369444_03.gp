M=40;

a365730(n) = sum(k=0, n\4, binomial(k, n-4*k)*binomial(n+1, k))/(n+1);
for(n=0, M, print1(a365730(n),", "))  

\\ a(n) = Sum_{k=0..n} binomial(n,k) * A365730(k). 
a(n) = sum(k=0, n, binomial(n, k)*a365730(k));
for(n=0, M, print1(a(n),", "))

