M=21;

\\ a(n) = n! * Sum_{k=0..n} 3^k * k^(n-k) * binomial(k/3+1,k)/( (k/3+1)*(n-k)! ).
a(n) = n! * sum(k=0, n, 3^k * k^(n-k) * binomial(k/3+1,k)/( (k/3+1)*(n-k)! ));
for(n=0, M, print1(a(n),", "));

