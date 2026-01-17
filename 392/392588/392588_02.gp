\\ a(n) = Sum_{k=0..floor(n/4)} (k+1) * binomial(n+7,n-4*k).
b(n) = sum(k=0, n\4, (k+1) * binomial(n+7, n-4*k));
for(n=0, 30, print1(b(n), ", "));

\\ a(n) = 8*a(n-1) - 28*a(n-2) + 56*a(n-3) - 68*a(n-4) + 48*a(n-5) - 16*a(n-6).
a(n) = if(n<6, b(n), 8*a(n-1) - 28*a(n-2) + 56*a(n-3) - 68*a(n-4) + 48*a(n-5) - 16*a(n-6));
for(n=0, 20, print1(a(n)-b(n), ", "));

