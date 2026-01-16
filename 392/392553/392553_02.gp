\\ a(n) = Sum_{k=0..floor(n/5)} (k+1) * binomial(n-3*k+3,n-5*k).
b(n) = sum(k=0, n\5, (k+1) * binomial(n-3*k+3, n-5*k));
for(n=0, 36, print1(b(n), ", "));

\\ a(n) = 4*a(n-1) - 6*a(n-2) + 4*a(n-3) - a(n-4) + 2*a(n-5) - 4*a(n-6) + 2*a(n-7) - a(n-10).
a(n) = if(n<10, b(n), 4*a(n-1) - 6*a(n-2) + 4*a(n-3) - a(n-4) + 2*a(n-5) - 4*a(n-6) + 2*a(n-7) - a(n-10));
for(n=0, 20, print1(a(n)-b(n), ", "));

