\\ a(n) = (1/2) * Sum_{k=0..n} 2^k * binomial(2*k+2,2*n-2*k+1).
b(n) = (1/2) * sum(k=0, n, 2^k * binomial(2*k+2,2*n-2*k+1));
for(n=0, 24, print1(b(n),", "));

\\ a(n) = 4*a(n-1) + 8*a(n-3) - 4*a(n-4).
a(n) = if(n<4, b(n), 4*a(n-1) + 8*a(n-3) - 4*a(n-4));
for(n=0, 30, print1(a(n)-b(n),", "));
