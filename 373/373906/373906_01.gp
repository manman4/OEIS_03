\\ a(n) = Sum_{k=0..floor(n/4)} binomial(n+2*k,n-4*k).
b(n) = sum(k=0, n\4, binomial(n+2*k, n-4*k));
for(n=0, 33, print1(b(n),", "))

a(n) = if(n<6, b(n), 6*a(n-1) - 15*a(n-2) + 20*a(n-3) - 14*a(n-4) + 6*a(n-5) - a(n-6));
for(n=0, 25, print1(a(n)-b(n), ", "))