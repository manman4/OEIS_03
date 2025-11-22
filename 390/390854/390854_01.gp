M=31;
\\ a(n) = Sum_{k=0..n} (-1)^k * binomial(n+2*k-1,n-k) * Fibonacci(k+1).
b(n) = sum(k=0, n, (-1)^k * binomial(n+2*k-1, n-k) * fibonacci(k+1));
for(n=0, M, print1(b(n),", ")); 

\\ a(n) = 5*a(n-1) - 11*a(n-2) + 17*a(n-3) - 14*a(n-4) + 6*a(n-5) - a(n-6) だと駄目
a(n) = if(n<6, b(n), 5*a(n-1) - 11*a(n-2) + 17*a(n-3) - 14*a(n-4) + 6*a(n-5) - a(n-6));
for(n=0, 15, print1(a(n)-b(n),", "));

\\　a(n) = 5*a(n-1) - 11*a(n-2) + 17*a(n-3) - 14*a(n-4) + 6*a(n-5) - a(n-6) for n > 6.
a(n) = if(n<7, b(n), 5*a(n-1) - 11*a(n-2) + 17*a(n-3) - 14*a(n-4) + 6*a(n-5) - a(n-6));
for(n=0, 15, print1(a(n)-b(n),", "));