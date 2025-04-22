M=17;

\\ a(n) = Sum_{k=0..n} (2*k+1) * (1/2)^(n+k) * (2*k)! * (n-k)! * binomial(n,k)^2.
a(n) = sum(k=0, n, (2*k+1) * (1/2)^(n+k) * (2*k)! * (n-k)! * binomial(n,k)^2);
for(n=0, M, print1(a(n),", "))