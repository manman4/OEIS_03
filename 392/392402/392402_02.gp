\\ a(n) = Sum_{k=0..floor(n/3)} binomial(2*n-3*k,3*k).
b(n) = sum(k=0, n\3, binomial(2*n-3*k, 3*k));
for(n=0, 30, print1(b(n), ", "));

\\ a(n) = 3*a(n-1) - 3*a(n-2) + 3*a(n-3) + 6*a(n-4) - a(n-6).
a(n) = if(n<6, b(n), 3*a(n-1) - 3*a(n-2) + 3*a(n-3) + 6*a(n-4) - a(n-6));
for(n=0, 20, print1(a(n)-b(n), ", "));

