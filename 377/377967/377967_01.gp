M=20;

\\ a(n) = n! * Sum_{k=0..n} binomial(3*k+3,n-k) / k!.
a(n) = n! * sum(k=0, n, binomial(3*k+3, n-k)/k!);
for(n=0, M, print1(a(n),", "))                 

