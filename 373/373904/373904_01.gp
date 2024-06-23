\\ a(n) = Sum_{k=0..floor(n/2)} binomial(n+4*k,n-2*k).
b(n) = sum(k=0, n\2, binomial(n+4*k, n-2*k));
for(n=0, 28, print1(b(n),", "))

a(n) = if(n<6, b(n), 6*a(n-1) - 14*a(n-2) + 20*a(n-3) - 15*a(n-4) + 6*a(n-5) - a(n-6));
for(n=0, 25, print1(a(n)-b(n), ", "))