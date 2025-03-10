M=20;

a185951(n, k) = binomial(n, k)/2^k*sum(j=0, k, (2*j-k)^(n-k)*binomial(k, j));
for(n=0, 10, for(k=0, n, print1(a185951(n, k),", ")); print);

\\ a(n) = (1/(n+1)) * Sum_{k=0..n} k! * binomial(n+1,k) * i^(n-k) * A185951(n,k), where i is the imaginary unit.
a(n) = (1/(n+1)) * sum(k=0, n, k! * binomial(n+1, k) * I^(n-k) * a185951(n, k));
for(n=0, M, print1(a(n), ", "));



