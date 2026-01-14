\\ a(n) = Sum_{k=0..floor(n/2)} binomial(k+1,2*n-4*k+1).
b(n) = sum(k=0, n\2, binomial(k+1, 2*n-4*k+1));
for(n=0, 48, print1(b(n), ", "));

\\ a(n) = 2*a(n-2) - a(n-4) + a(n-5).
a(n) = if(n<5, b(n), 2*a(n-2) - a(n-4) + a(n-5));
for(n=0, 20, print1(a(n)-b(n), ", "));

