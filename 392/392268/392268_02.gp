\\ a(n) = Sum_{k=0..floor(2*n/5)} (k+1) * binomial(k,2*n-5*k).
b(n) = sum(k=0, (2*n)\5, (k+1) * binomial(k, 2*n-5*k));
for(n=0, 49, print1(b(n), ", "));

\\ a(n) = 4*a(n-3) + 2*a(n-5) - 6*a(n-6) - 4*a(n-8) + 4*a(n-9) - a(n-10) + 2*a(n-11) - a(n-12).
a(n) = if(n<12, b(n), 4*a(n-3) + 2*a(n-5) - 6*a(n-6) - 4*a(n-8) + 4*a(n-9) - a(n-10) + 2*a(n-11) - a(n-12));
for(n=0, 25, print1(a(n)-b(n), ", "));

