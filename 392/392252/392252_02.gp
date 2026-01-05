\\ a(n) = Sum_{k=0..floor(n/2)} binomial(k+2,2) * binomial(k,2*(n-2*k)).
b(n) = sum(k=0, n\2, binomial(k+2,2) * binomial(k, 2*(n-2*k)));
for(n=0, 43, print1(b(n), ", "));

\\ a(n) = 6*a(n-2) - 15*a(n-4) + 3*a(n-5) + 20*a(n-6) - 12*a(n-7) - 15*a(n-8) + 18*a(n-9) + 3*a(n-10) - 12*a(n-11) + 5*a(n-12) + 3*a(n-13) - 3*a(n-14) + a(n-15).
a(n) = if(n<15, b(n), 6*a(n-2) - 15*a(n-4) + 3*a(n-5) + 20*a(n-6) - 12*a(n-7) - 15*a(n-8) + 18*a(n-9) + 3*a(n-10) - 12*a(n-11) + 5*a(n-12) + 3*a(n-13) - 3*a(n-14) + a(n-15));
for(n=0, 25, print1(a(n)-b(n), ", "));

