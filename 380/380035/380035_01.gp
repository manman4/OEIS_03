M=18;

\\ a(n) = n! * Sum_{k=0..n} 2^k * k^(n-k) * binomial(n/2+k/2+1/2,k)/( (n+k+1)*(n-k)! ).
a(n) = n! * sum(k=0, n, 2^k * k^(n-k) * binomial(n/2+k/2+1/2,k) / (n-k)! / (n+k+1));
for(n=0, M, print1(a(n),", "));

