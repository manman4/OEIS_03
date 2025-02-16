M=21;

a185951(n, k) = binomial(n, k)/2^k*sum(j=0, k, (2*j-k)^(n-k)*binomial(k, j));
for(n=0, 10, for(k=0, n, print1(a185951(n, k),", ")); print);

\\ a(n) = Sum_{k=0..n} k! * binomial(n-k+1,k)/(n-k+1) * i^(n-k) * A185951(n,k), where i is the imaginary unit.
a(n) = sum(k=0, n, k! * binomial(n-k+1, k)/(n-k+1) * I^(n-k) * a185951(n, k));
for(n=0, M, print1(a(n), ", "));



