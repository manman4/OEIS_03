M=22;

a004212(n) = sum(k=0, n, 3^(n-k)*stirling(n, k, 2));
for(n=0, M, print1(a004212(n), ", "));

\\ a(n) = Sum_{k=0..n} 2^(n-k) * Stirling1(n,k) * A004212(k) 
a(n) = sum(k=0, n, 2^(n-k) * stirling(n, k, 1) * a004212(k));  
for(n=0, M, print1(a(n), ", "));

bell(n, x) = sum(k=0, n, x^k * stirling(n, k, 2));

\\ Sum_{k=0..n} 3^k * 2^(n-k) * Stirling1(n,k) * Bell_k(1/3)
b(n) = sum(k=0, n, 3^k * 2^(n-k) * stirling(n, k, 1) * bell(k, 1/3));
for(n=0, 50, print1(a(n)-b(n), ", "));

\\ a(n) = (1/exp(1/3)) * 2^n * n! * Sum_{k>=0} binomial(3*k/2,n)/(3^k * k!).
c(n) = round( (1/exp(1/3)) * 2^n * n! * sum(k=0, 1000, binomial(3*k/2, n)/(3^k * k!)) );
for(n=0, M, print1(a(n)-c(n), ", "));