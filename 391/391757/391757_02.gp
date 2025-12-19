\\ a(n) = Sum_{k=0..floor(n/4)} 2^k * 3^(n-3*k) * binomial(2*(n-3*k),2*k).
b(n) = sum(k=0, n\4, 2^k * 3^(n-3*k) * binomial(2*(n-3*k),2*k));
for(n=0, 25, print1(b(n), ", "));

\\ a(n) = 6*a(n-1) - 9*a(n-2) + 12*a(n-4) + 36*a(n-5) - 36*a(n-8).
a(n) = if(n<8, b(n), 6*a(n-1) - 9*a(n-2) + 12*a(n-4) + 36*a(n-5) - 36*a(n-8));
for(n=0, 20, print1(a(n)-b(n), ", "));

