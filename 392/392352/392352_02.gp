\\ a(n) = Sum_{k=0..floor(3*n/17)} binomial(3*n-16*k,k).
b(n) = sum(k=0, (3*n)\17, binomial(3*n-16*k, k));
for(n=0, 45, print1(b(n), ", "));

\\ a(n) = 2*a(n-1) - 2*a(n-2) + 2*a(n-3) - 2*a(n-4) + 2*a(n-5) + a(n-6) - a(n-7) + a(n-8) - a(n-9) + a(n-10) - a(n-11) + a(n-12) - a(n-13) + a(n-14) - a(n-15) + a(n-16).
a(n) = if(n<16, b(n), 2*a(n-1) - 2*a(n-2) + 2*a(n-3) - 2*a(n-4) + 2*a(n-5) + a(n-6) - a(n-7) + a(n-8) - a(n-9) + a(n-10) - a(n-11) + a(n-12) - a(n-13) + a(n-14) - a(n-15) + a(n-16));
for(n=0, 25, print1(a(n)-b(n), ", "));

