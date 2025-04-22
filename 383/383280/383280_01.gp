M=17;

\\ a(n) = (3/2)^n * Sum_{k=0..n} (1/6)^k * (2*k)! * (n-k)! * binomial(n,k)^2.
a(n) = (3/2)^n * sum(k=0, n, (1/6)^k * (2*k)! * (n-k)! * binomial(n,k)^2);
for(n=0, M, print1(a(n),", "))