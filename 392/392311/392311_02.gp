\\ a(n) = Sum_{k=0..floor(3*n/5)} (k+1) * binomial(k,3*n-5*k).
b(n) = sum(k=0, (3*n)\5, (k+1) * binomial(k, 3*n-5*k));
for(n=0, 42, print1(b(n), ", "));

\\ a(n) = 6*a(n-2) - 15*a(n-4) + 2*a(n-5) + 20*a(n-6) - 6*a(n-7) - 15*a(n-8) + 6*a(n-9) + 5*a(n-10) - 2*a(n-11) - a(n-12).
a(n) = if(n<12, b(n), 6*a(n-2) - 15*a(n-4) + 2*a(n-5) + 20*a(n-6) - 6*a(n-7) - 15*a(n-8) + 6*a(n-9) + 5*a(n-10) - 2*a(n-11) - a(n-12));
for(n=0, 25, print1(a(n)-b(n), ", "));

