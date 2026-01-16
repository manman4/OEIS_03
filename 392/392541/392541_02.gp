\\ a(n) = Sum_{k=0..floor(n/6)} binomial(n-4*k+1,n-6*k).
b(n) = sum(k=0, n\6, binomial(n-4*k+1, n-6*k));
for(n=0, 44, print1(b(n), ", "));

\\ a(n) = 2*a(n-1) - a(n-2) + a(n-6).
a(n) = if(n<6, b(n), 2*a(n-1) - a(n-2) + a(n-6));
for(n=0, 20, print1(a(n)-b(n), ", "));

