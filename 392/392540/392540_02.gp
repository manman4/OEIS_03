\\ a(n) = Sum_{k=0..floor(n/5)} binomial(n-3*k+1,n-5*k).
b(n) = sum(k=0, n\5, binomial(n-3*k+1, n-5*k));
for(n=0, 41, print1(b(n), ", "));

\\ a(n) = 2*a(n-1) - a(n-2) + a(n-5).
a(n) = if(n<5, b(n), 2*a(n-1) - a(n-2) + a(n-5));
for(n=0, 20, print1(a(n)-b(n), ", "));

