\\ a(n) = Sum_{k=0..floor(3*n/5)} binomial(k+2,3*n-5*k).
b(n) = sum(k=0, (3*n)\5, binomial(k+2, 3*n-5*k));
for(n=0, 43, print1(b(n), ", "));

\\ a(n) = 3*a(n-2) - 3*a(n-4) + a(n-5) + a(n-6).
a(n) = if(n<6, b(n), 3*a(n-2) - 3*a(n-4) + a(n-5) + a(n-6));
for(n=0, 25, print1(a(n)-b(n), ", "));

