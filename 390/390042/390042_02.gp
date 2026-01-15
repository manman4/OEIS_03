\\ a(n) = Sum_{k=0..floor(n/3)} binomial(k+3,4*n-12*k+3).
b(n) = sum(k=0, n\3, binomial(k+3, 4*n-12*k+3));
for(n=0, 55, print1(b(n), ", "));

\\ a(n) = 4*a(n-3) - 6*a(n-6) + 4*a(n-9) - a(n-12) + a(n-13).
a(n) = if(n<13, b(n), 4*a(n-3) - 6*a(n-6) + 4*a(n-9) - a(n-12) + a(n-13));
for(n=0, 20, print1(a(n)-b(n), ", "));

