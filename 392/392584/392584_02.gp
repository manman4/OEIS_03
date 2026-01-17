\\ a(n) = Sum_{k=0..floor(n/3)} (k+1) * binomial(n+5,n-3*k).
b(n) = sum(k=0, n\3, (k+1) * binomial(n+5, n-3*k));
for(n=0, 31, print1(b(n), ", "));

\\ a(n) = 6*a(n-1) - 15*a(n-2) + 22*a(n-3) - 21*a(n-4) + 12*a(n-5) - 4*a(n-6).
a(n) = if(n<6, b(n), 6*a(n-1) - 15*a(n-2) + 22*a(n-3) - 21*a(n-4) + 12*a(n-5) - 4*a(n-6));
for(n=0, 20, print1(a(n)-b(n), ", "));

