\\ a(n) = Sum_{k=0..floor(n/2)} binomial(2*n-k,3*k).
b(n) = sum(k=0, n\2, binomial(2*n-k, 3*k));
for(n=0, 27, print1(b(n), ", "));

\\ a(n) = 3*a(n-1) - a(n-2) + 7*a(n-3) - a(n-4).
a(n) = if(n<6, b(n), 3*a(n-1) - a(n-2) + 7*a(n-3) - a(n-4));
for(n=0, 20, print1(a(n)-b(n), ", "));

