\\ a(n) = Sum_{k=0..floor(n/7)} binomial(n-k+5,n-7*k).
b(n) = sum(k=0, n\7, binomial(n-k+5, n-7*k));
for(n=0, 32, print1(b(n), ", "));

\\ a(n) = 6*a(n-1) - 15*a(n-2) + 20*a(n-3) - 15*a(n-4) + 6*a(n-5) - a(n-6) + a(n-7).
a(n) = if(n<7, b(n), 6*a(n-1) - 15*a(n-2) + 20*a(n-3) - 15*a(n-4) + 6*a(n-5) - a(n-6) + a(n-7));
for(n=0, 20, print1(a(n)-b(n), ", "));

