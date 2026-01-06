\\ a(n) = Sum_{k=0..floor(2*n/5)} binomial(k+2,2) * binomial(k,2*n-5*k).
b(n) = sum(k=0, (2*n)\5, binomial(k+2, 2) * binomial(k, 2*n-5*k));
for(n=0, 44, print1(b(n), ", "));

\\ a(n) = 6*a(n-3) + 3*a(n-5) - 15*a(n-6) - 12*a(n-8) + 20*a(n-9) - 3*a(n-10) + 18*a(n-11) - 15*a(n-12) + 6*a(n-13) - 12*a(n-14) + 7*a(n-15) - 3*a(n-16) + 3*a(n-17) - a(n-18).
a(n) = if(n<18, b(n), 6*a(n-3) + 3*a(n-5) - 15*a(n-6) - 12*a(n-8) + 20*a(n-9) - 3*a(n-10) + 18*a(n-11) - 15*a(n-12) + 6*a(n-13) - 12*a(n-14) + 7*a(n-15) - 3*a(n-16) + 3*a(n-17) - a(n-18));
for(n=0, 25, print1(a(n)-b(n), ", "));

