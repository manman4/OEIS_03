\\ a(n) = Sum_{k=0..floor(n/5)} binomial(n+2*k,n-5*k).
b(n) = sum(k=0, n\5, binomial(n+2*k, n-5*k));
for(n=0, 33, print1(b(n),", "))

a(n) = if(n<7, b(n), 7*a(n-1) - 21*a(n-2) + 35*a(n-3) - 35*a(n-4) + 22*a(n-5) - 7*a(n-6) + a(n-7));
for(n=0, 25, print1(a(n)-b(n), ", "))