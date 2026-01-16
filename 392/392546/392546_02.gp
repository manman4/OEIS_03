\\ a(n) = Sum_{k=0..floor(n/7)} binomial(n-2*k+4,n-7*k).
b(n) = sum(k=0, n\7, binomial(n-2*k+4, n-7*k));
for(n=0, 35, print1(b(n), ", "));

\\ a(n) = 5*a(n-1) - 10*a(n-2) + 10*a(n-3) - 5*a(n-4) + a(n-5) + a(n-7).
a(n) = if(n<7, b(n), 5*a(n-1) - 10*a(n-2) + 10*a(n-3) - 5*a(n-4) + a(n-5) + a(n-7));
for(n=0, 20, print1(a(n)-b(n), ", "));

