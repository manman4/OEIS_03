\\ a(n) = Sum_{k=0..floor(n/3)} binomial(k,2*n-6*k).
b(n) = sum(k=0, n\3, binomial(k, 2*n-6*k));
for(n=0, 58, print1(b(n), ", "));

\\ a(n) = 2*a(n-3) - a(n-6) + a(n-7).
a(n) = if(n<7, b(n), 2*a(n-3) - a(n-6) + a(n-7));
for(n=0, 20, print1(a(n)-b(n), ", "));

