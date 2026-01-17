\\ a(n) = Sum_{k=0..floor(n/2)} (k+1) * binomial(n+k+5,n-2*k).
b(n) = sum(k=0, n\2, (k+1) * binomial(n+k+5, n-2*k));
for(n=0, 28, print1(b(n), ", "));

\\ a(n) = 6*a(n-1) - 13*a(n-2) + 14*a(n-3) - 10*a(n-4) + 4*a(n-5) - a(n-6).
a(n) = if(n<6, b(n), 6*a(n-1) - 13*a(n-2) + 14*a(n-3) - 10*a(n-4) + 4*a(n-5) - a(n-6));
for(n=0, 20, print1(a(n)-b(n), ", "));

