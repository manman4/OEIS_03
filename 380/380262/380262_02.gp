M=18;

a004211(n) = sum(k=0, n, 2^(n-k)*stirling(n, k, 2));
for(n=0, M, print1(a004211(n), ", "));

\\ a(n) = Sum_{k=0..n} 5^(n-k) * Stirling1(n,k) * A004211(k)
a(n) = sum(k=0, n, 5^(n-k) * stirling(n, k, 1) * a004211(k));
for(n=0, M, print1(a(n), ", "));

bell(n, x) = sum(k=0, n, x^k * stirling(n, k, 2));

\\ Sum_{k=0..n} 2^k * 5^(n-k) * Stirling1(n,k) * Bell_k(1/2)
b(n) = sum(k=0, n, 2^k * 5^(n-k) * stirling(n, k, 1) * bell(k, 1/2));
for(n=0, 50, print1(a(n)-b(n), ", "));

\\ a(n) = (1/exp(1/2)) * 5^n * n! * Sum_{k>=0} binomial(2*k/5,n)/(2^k * k!).
c(n) = round( (1/exp(1/2)) * 5^n * n! * sum(k=0, 1000, binomial(2*k/5, n)/(2^k * k!)) );     
for(n=0, M, print1(a(n)-c(n), ", "));