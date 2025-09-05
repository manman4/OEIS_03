\\ a(n) = Sum_{k=0..floor(n/2)} 2^(n-2*k) * binomial(2*k+1,2*n-4*k+1).
b(n) = sum(k=0, n\2, 2^(n-2*k) * binomial(2*k+1, 2*n-4*k+1));
for(n=0, 35, print1(b(n),", "));

\\ a(n) = 2*a(n-2) + 4*a(n-3) - a(n-4) + 4*a(n-5) - 4*a(n-6).
a(n) = if(n<6, b(n), 2*a(n-2) + 4*a(n-3) - a(n-4) + 4*a(n-5) - 4*a(n-6));
for(n=0, 25, print1(a(n)-b(n),", "));
