\\ a(n) = Sum_{k=0..n} binomial(2*n+1,n-k) * Fibonacci(k+1).
a(n) = sum(k=0, n, binomial(2*n+1, n-k) * fibonacci(k+1));
for(n=0, 24, print1(a(n),", "));   


