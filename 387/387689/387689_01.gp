\\ a(n) = Sum_{k=0..floor(n/2)} 2^(n-2*k) * binomial(2*n-2*k,2*k).
b(n) = sum(k=0, n\2, 2^(n-2*k) * binomial(2*n-2*k, 2*k));
for(n=0, 26, print1(b(n),", "));

\\ a(n) = 4*a(n-1) - 2*a(n-2) + 4*a(n-3) - a(n-4).
a(n) = if(n<4, b(n), 4*a(n-1) - 2*a(n-2) + 4*a(n-3) - a(n-4));
for(n=0, 25, print1(a(n)-b(n),", "));

\\ a(n) = A387694(n-1) + (-1)^floor(n/2).
a387694(n) = sum(k=0, n\2, 2^(n-2*k) * binomial(2*n-2*k+1, 2*k+1));
for(n=0, 50, print1(a387694(n-1) + (-1)^(n\2) - b(n),", "));
