\\ a(n) = Sum_{k=0..floor(n/6)} binomial(n-2*k+3,n-6*k).
b(n) = sum(k=0, n\6, binomial(n-2*k+3, n-6*k));
for(n=0, 35, print1(b(n), ", "));

\\ a(n) = 4*a(n-1) - 6*a(n-2) + 4*a(n-3) - a(n-4) + a(n-6).
a(n) = if(n<6, b(n), 4*a(n-1) - 6*a(n-2) + 4*a(n-3) - a(n-4) + a(n-6));
for(n=0, 20, print1(a(n)-b(n), ", "));

