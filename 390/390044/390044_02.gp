\\ a(n) = Sum_{k=0..floor(n/5)} binomial(n-k+3,n-5*k).
b(n) = sum(k=0, n\5, binomial(n-k+3, n-5*k));
for(n=0, 34, print1(b(n), ", "));

\\ a(n) = 4*a(n-1) - 6*a(n-2) + 4*a(n-3) - a(n-4) + a(n-5).
a(n) = if(n<5, b(n), 4*a(n-1) - 6*a(n-2) + 4*a(n-3) - a(n-4) + a(n-5));
for(n=0, 20, print1(a(n)-b(n), ", "));

