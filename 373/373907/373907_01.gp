\\ a(n) = Sum_{k=0..n} binomial(n+6*k,n-k).
b(n) = sum(k=0, n, binomial(n+6*k, n-k));
for(n=0, 23, print1(b(n),", "))

a(n) = if(n<7, b(n), 8*a(n-1) - 21*a(n-2) + 35*a(n-3) - 35*a(n-4) + 21*a(n-5) - 7*a(n-6) + a(n-7));
for(n=0, 25, print1(a(n)-b(n), ", "))