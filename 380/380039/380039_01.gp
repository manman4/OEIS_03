M=18;

\\ a(n) = n! * Sum_{k=0..n} 3^k * k^(n-k) * binomial(n/3+2*k/3+1/3,k)/( (n+2*k+1)*(n-k)! ).
a(n) = n! * sum(k=0, n, 3^k * k^(n-k) * binomial(n/3+2*k/3+1/3,k)/( (n+2*k+1)*(n-k)! ));
for(n=0, M, print1(a(n),", "));

