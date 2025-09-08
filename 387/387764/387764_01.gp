\\ a(n) = Sum_{k=0..n} 2^k * binomial(2*k+1,2*n-2*k).
b(n) = sum(k=0, n, 2^k * binomial(2*k+1, 2*n-2*k));
for(n=0, 24, print1(b(n),", "));

\\ a(n) = 4*a(n-1) + 8*a(n-3) - 4*a(n-4).
a(n) = if(n<4, b(n), 4*a(n-1) + 8*a(n-3) - 4*a(n-4));
for(n=0, 25, print1(a(n)-b(n),", "));
