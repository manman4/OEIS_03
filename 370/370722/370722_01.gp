M=42;

\\ a(n) = Sum_{k=0..floor(n/7)} binomial(n-4*k,3*k).
b(n) = sum(k=0, n\7, binomial(n-4*k,3*k));
for(n=0, M, print1(b(n), ", "))


a(n) = if(n<7, 1, 3*a(n-1) - 3*a(n-2) + a(n-3) + a(n-7) );
for(n=0, 30, print1(a(n)-b(n), ", "))