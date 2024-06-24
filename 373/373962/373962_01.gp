\\ a(n) = Sum_{k=0..floor(n/5)} binomial(n+k,n-2-5*k).
b(n) = sum(k=0, n\5, binomial(n+k, n-2-5*k));
for(n=1, 34, print1(b(n),", "))

a(n) = if(n<6, b(n), 6*a(n-1) - 15*a(n-2) + 20*a(n-3) - 15*a(n-4) + 7*a(n-5) - a(n-6));
for(n=0, 25, print1(a(n)-b(n), ", "))