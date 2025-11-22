M=33;
\\ a(n) = Sum_{k=0..n} (-1)^k * binomial(n+k+1,n-k) * Fibonacci(k+1).
b(n) = sum(k=0, n, (-1)^k * binomial(n+k+1, n-k) * fibonacci(k+1));
for(n=0, M, print1(b(n),", ")); 

\\ a(n) = 3*a(n-1) - 3*a(n-2) + 3*a(n-3) - a(n-4).
a(n) = if(n<4, b(n), 3*a(n-1) - 3*a(n-2) + 3*a(n-3) - a(n-4));
for(n=0, 15, print1(a(n)-b(n),", "));