\\ a(n) = Sum_{k=0..floor(2*n/3)} binomial(k+2,2) * binomial(k,2*n-3*k).
b(n) = sum(k=0, (2*n)\3, binomial(k+2, 2) * binomial(k, 2*n-3*k));
for(n=0, 36, print1(b(n), ", "));

\\ a(n) = 6*a(n-2) + 3*a(n-3) - 15*a(n-4) - 12*a(n-5) + 17*a(n-6) + 18*a(n-7) - 9*a(n-8) - 11*a(n-9) + 3*a(n-10) + 3*a(n-11) - a(n-12).
a(n) = if(n<12, b(n), 6*a(n-2) + 3*a(n-3) - 15*a(n-4) - 12*a(n-5) + 17*a(n-6) + 18*a(n-7) - 9*a(n-8) - 11*a(n-9) + 3*a(n-10) + 3*a(n-11) - a(n-12));
for(n=0, 25, print1(a(n)-b(n), ", "));

