\\ a(n) = Sum_{k=0..floor(n/7)} binomial(n-5*k+1,n-7*k).
b(n) = sum(k=0, n\7, binomial(n-5*k+1, n-7*k));
for(n=0, 44, print1(b(n), ", "));

\\ a(n) = 2*a(n-1) - a(n-2) + a(n-7).
a(n) = if(n<7, b(n), 2*a(n-1) - a(n-2) + a(n-7));
for(n=0, 20, print1(a(n)-b(n), ", "));

