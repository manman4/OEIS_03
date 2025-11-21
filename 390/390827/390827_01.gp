M=25;
\\ a(n) = Sum_{k=0..n} binomial(n+2*k+2,n-k) * Fibonacci(k+1).
b(n) = sum(k=0, n, binomial(n+2*k+2, n-k) * fibonacci(k+1));
for(n=0, M, print1(b(n),", ")); 

\\ a(n) = 7*a(n-1) - 17*a(n-2) + 23*a(n-3) - 16*a(n-4) + 6*a(n-5) - a(n-6).
a(n) = if(n<6, b(n), 7*a(n-1) - 17*a(n-2) + 23*a(n-3) - 16*a(n-4) + 6*a(n-5) - a(n-6));
for(n=0, 15, print1(a(n)-b(n),", "));