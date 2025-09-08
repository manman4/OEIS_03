\\ a(n) = Sum_{k=0..floor(n/3)} 2^k * binomial(2*k+1,2*n-6*k).
b(n) = sum(k=0, n\3, 2^k * binomial(2*k+1, 2*n-6*k));
for(n=0, 37, print1(b(n),", "));

\\ a(n) = 4*a(n-3) + 4*a(n-4) - 4*a(n-6) + 8*a(n-7) - 4*a(n-8).
a(n) = if(n<8, b(n), 4*a(n-3) + 4*a(n-4) - 4*a(n-6) + 8*a(n-7) - 4*a(n-8));
for(n=0, 25, print1(a(n)-b(n),", "));
