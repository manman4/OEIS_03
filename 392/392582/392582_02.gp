\\ a(n) = Sum_{k=0..floor(n/2)} (k+1) * binomial(n+2*k+7,n-2*k).
b(n) = sum(k=0, n\2, (k+1) * binomial(n+2*k+7, n-2*k));
for(n=0, 26, print1(b(n), ", "));

\\ a(n) = 8*a(n-1) - 26*a(n-2) + 48*a(n-3) - 59*a(n-4) + 48*a(n-5) - 26*a(n-6) + 8*a(n-7) - a(n-8).
a(n) = if(n<8, b(n), 8*a(n-1) - 26*a(n-2) + 48*a(n-3) - 59*a(n-4) + 48*a(n-5) - 26*a(n-6) + 8*a(n-7) - a(n-8));
for(n=0, 20, print1(a(n)-b(n), ", "));

