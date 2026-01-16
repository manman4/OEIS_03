\\ a(n) = Sum_{k=0..floor(n/4)} (k+1) * binomial(n-2*k+3,n-4*k).
b(n) = sum(k=0, n\4, (k+1) * binomial(n-2*k+3, n-4*k));
for(n=0, 35, print1(b(n), ", "));

\\ a(n) = 4*a(n-1) - 6*a(n-2) + 4*a(n-3) + a(n-4) - 4*a(n-5) + 2*a(n-6) - a(n-8).
a(n) = if(n<8, b(n), 4*a(n-1) - 6*a(n-2) + 4*a(n-3) + a(n-4) - 4*a(n-5) + 2*a(n-6) - a(n-8));
for(n=0, 20, print1(a(n)-b(n), ", "));

