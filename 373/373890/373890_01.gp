\\ a(n) = Sum_{k=0..n} binomial(n+7*k,n-k).
b(n) = sum(k=0, n, binomial(n+7*k, n-k));
for(n=0, 23, print1(b(n),", "))

a(n) = if(n<8, b(n), 9*a(n-1) - 28*a(n-2) + 56*a(n-3) - 70*a(n-4) + 56*a(n-5) - 28*a(n-6) + 8*a(n-7) - a(n-8));
for(n=0, 25, print1(a(n)-b(n), ", "))