\\ a(n) = Sum_{k=0..floor(2*n/3)} binomial(3*k,2*n-3*k).
b(n) = sum(k=0, (2*n)\3, binomial(3*k, 2*n-3*k));
for(n=0, 30, print1(b(n), ", "));

\\ a(n) = 6*a(n-2) + 3*a(n-3) - 3*a(n-4) + 3*a(n-5) - a(n-6).
a(n) = if(n<6, b(n), 6*a(n-2) + 3*a(n-3) - 3*a(n-4) + 3*a(n-5) - a(n-6));
for(n=0, 20, print1(a(n)-b(n), ", "));

