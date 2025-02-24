M=18;

a185951(n, k) = binomial(n, k)/2^k*sum(j=0, k, (2*j-k)^(n-k)*binomial(k, j));
\\ a(n) = (1/(3*n+1)) * Sum_{k=0..n} k! * binomial(3*n+1,k) * A185951(n,k).
a(n) = sum(k=0, n, k! * binomial(3*n+1,k) * a185951(n, k))/(3*n+1);
for(n=0, M, print1(a(n),", "));

