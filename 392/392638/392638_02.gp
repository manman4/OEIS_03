\\ a(n) = Sum_{k=0..floor(n/5)} (k+1) * binomial(n-k+7,n-5*k).
b(n) = sum(k=0, n\5, (k+1) * binomial(n-k+7, n-5*k));
for(n=0, 31, print1(b(n), ", "));

\\ a(n) = 8*a(n-1) - 28*a(n-2) + 56*a(n-3) - 70*a(n-4) + 58*a(n-5) - 36*a(n-6) + 20*a(n-7) - 9*a(n-8) + 2*a(n-9) - a(n-10).
a(n) = if(n<10, b(n), 8*a(n-1) - 28*a(n-2) + 56*a(n-3) - 70*a(n-4) + 58*a(n-5) - 36*a(n-6) + 20*a(n-7) - 9*a(n-8) + 2*a(n-9) - a(n-10));
for(n=0, 20, print1(a(n)-b(n), ", "));

