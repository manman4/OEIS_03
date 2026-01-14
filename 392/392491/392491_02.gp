\\ a(n) = Sum_{k=0..floor(n/4)} binomial(k+1,2*n-8*k+1).
b(n) = sum(k=0, n\4, binomial(k+1, 2*n-8*k+1));
for(n=0, 64, print1(b(n), ", "));

\\ a(n) = 2*a(n-4) - a(n-8) + a(n-9).
a(n) = if(n<9, b(n), 2*a(n-4) - a(n-8) + a(n-9));
for(n=0, 20, print1(a(n)-b(n), ", "));

