\\ a(n) = (1/2) * Sum_{k=0..floor(n/3)} 2^(n-2*k) * binomial(2*n-4*k+2,2*k+1).
b(n) = (1/2) * sum(k=0, n\3, 2^(n-2*k) * binomial(2*n-4*k+2,2*k+1));
for(n=0, 26, print1(b(n),", "));

\\ a(n) = 4*a(n-1) - 4*a(n-2) + 4*a(n-3) + 8*a(n-4) - 4*a(n-6).
a(n) = if(n<6, b(n), 4*a(n-1) - 4*a(n-2) + 4*a(n-3) + 8*a(n-4) - 4*a(n-6));
for(n=0, 30, print1(a(n)-b(n),", "));
