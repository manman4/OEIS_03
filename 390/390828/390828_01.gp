M=25;
\\ a(n) = Sum_{k=0..n} binomial(n+2*k+5,n-k) * Fibonacci(k+1).
a(n) = sum(k=0, n, binomial(n+2*k+5, n-k) * fibonacci(k+1));
for(n=0, M, print1(a(n),", "));  