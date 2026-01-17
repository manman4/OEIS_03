\\ a(n) = Sum_{k=0..floor(n/3)} (k+1) * binomial(n+k+7,n-3*k).
b(n) = sum(k=0, n\3, (k+1) * binomial(n+k+7, n-3*k));
for(n=0, 29, print1(b(n), ", "));

\\ a(n) = 8*a(n-1) - 28*a(n-2) + 58*a(n-3) - 78*a(n-4) + 68*a(n-5) - 37*a(n-6) + 10*a(n-7) - a(n-8).
a(n) = if(n<8, b(n), 8*a(n-1) - 28*a(n-2) + 58*a(n-3) - 78*a(n-4) + 68*a(n-5) - 37*a(n-6) + 10*a(n-7) - a(n-8));
for(n=0, 20, print1(a(n)-b(n), ", "));

