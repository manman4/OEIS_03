\\ a(n) = Sum_{k=0..floor(n/2)} binomial(n+5*k,n-2*k).
b(n) = sum(k=0, n\2, binomial(n+5*k, n-2*k));
for(n=0, 27, print1(b(n),", "))

a(n) = if(n<7, b(n), 7*a(n-1) - 20*a(n-2) + 35*a(n-3) - 35*a(n-4) + 21*a(n-5) - 7*a(n-6) + a(n-7));
for(n=0, 25, print1(a(n)-b(n), ", "))