M=14;

\\ a(n) = Fibonacci(n^2+1).
a(n) = fibonacci(n^2+1);
for(n=0, M, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n^2} binomial(k,n^2-k).
b(n) = sum(k=0, n^2, binomial(k, n^2 - k));
for(n=0, 100, print1(a(n)-b(n), ", "));
