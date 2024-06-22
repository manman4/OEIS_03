\\ a(n) = Sum_{k=0..floor(n/7)} binomial(n+k,n-7*k).
b(n) = sum(k=0, n\7, binomial(n+k, n-7*k));
for(n=0, 34, print1(b(n),", "))

a(n) = if(n<8, b(n), 8*a(n-1) - 28*a(n-2) + 56*a(n-3) - 70*a(n-4) + 56*a(n-5) - 28*a(n-6) + 9*a(n-7) - a(n-8));
for(n=0, 25, print1(a(n)-b(n), ", "))