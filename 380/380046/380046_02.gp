M=21;

\\ a(n) = n! * Sum_{k=0..n} 2^k * k^(n-k) * binomial(k/2+1,k)/( (k/2+1)*(n-k)! ).
a(n) = n! * sum(k=0, n, 2^k * k^(n-k) * binomial(k/2+1,k)/( (k/2+1)*(n-k)! ));
for(n=0, M, print1(a(n),", "));

