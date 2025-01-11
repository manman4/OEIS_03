M=18;

\\ a(n) = 2 * n! * Sum_{k=0..n} 3^k * k^(n-k) * binomial(2*n/3+k/3+2/3,k)/( (2*n+k+2)*(n-k)! ).
a(n) = 2 * n! * sum(k=0, n, 3^k * k^(n-k) * binomial(2*n/3+k/3+2/3,k)/( (2*n+k+2)*(n-k)! ));
for(n=0, M, print1(a(n),", "));

