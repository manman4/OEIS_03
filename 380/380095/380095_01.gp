M=17;

\\ a(n) = n! * Sum_{k=0..n} 2^k * k^(n-k) * binomial(n+k+1/2,k)/( (2*n+2*k+1)*(n-k)! ).
a(n) = n! * sum(k=0, n, 2^k * k^(n-k) * binomial(n+k+1/2,k)/( (2*n+2*k+1)*(n-k)! ));
for(n=0, M, print1(a(n),", "));



