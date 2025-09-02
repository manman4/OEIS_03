\\ a(n) = (1/2) * Sum_{k=0..floor(n/3)} 2^(n-k) * binomial(2*k+2,2*n-6*k+1).
b(n) = (1/2) * sum(k=0, n\3, 2^(n-k) * binomial(2*k+2,2*n-6*k+1));
for(n=0, 31, print1(b(n),", "));

\\ a(n) = 8*a(n-3) + 16*a(n-4) - 16*a(n-6) + 64*a(n-7) - 64*a(n-8).
a(n) = if(n<8, b(n), 8*a(n-3) + 16*a(n-4) - 16*a(n-6) + 64*a(n-7) - 64*a(n-8));
for(n=0, 40, print1(a(n)-b(n),", "));
