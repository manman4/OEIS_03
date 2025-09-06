\\ a(n) = Sum_{k=0..floor(n/4)} 2^(n-4*k) * binomial(2*n-6*k+1,2*k).
b(n) = sum(k=0, n\4, 2^(n-4*k) * binomial(2*n-6*k+1, 2*k));
for(n=0, 29, print1(b(n),", "));

\\ a(n) = 4*a(n-1) - 4*a(n-2) + 2*a(n-4) + 4*a(n-5) - a(n-8).
a(n) = if(n<8, b(n), 4*a(n-1) - 4*a(n-2) + 2*a(n-4) + 4*a(n-5) - a(n-8));
for(n=0, 25, print1(a(n)-b(n),", "));
