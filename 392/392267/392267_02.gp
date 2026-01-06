\\ a(n) = Sum_{k=0..floor(2*n/3)} (k+1) * binomial(k,2*n-3*k).
b(n) = sum(k=0, (2*n)\3, (k+1) * binomial(k, 2*n-3*k));
for(n=0, 39, print1(b(n), ", "));

\\ a(n) = 4*a(n-2) + 2*a(n-3) - 6*a(n-4) - 4*a(n-5) + 3*a(n-6) + 2*a(n-7) - a(n-8).
a(n) = if(n<8, b(n), 4*a(n-2) + 2*a(n-3) - 6*a(n-4) - 4*a(n-5) + 3*a(n-6) + 2*a(n-7) - a(n-8));
for(n=0, 25, print1(a(n)-b(n), ", "));

