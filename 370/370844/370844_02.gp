M=20;

a118971(n) = binomial(5*n+3,n)/(n+1);

\\ a(n) = Sum_{k=0..n} binomial(n,k) * A118971(k).
a(n) = sum(k=0, n, binomial(n,k) * a118971(k));
for(n=0, M, print1(a(n), ", "))