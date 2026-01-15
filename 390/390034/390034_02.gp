\\ a(n) = Sum_{k=0..floor(3*n/5)} binomial(k+2,3*n-5*k).
b(n) = sum(k=0, (3*n)\5, binomial(k+2, 3*n-5*k));
for(n=0, 43, print1(b(n), ", "));

\\ a(n) = 3*a(n-2) - 3*a(n-4) + a(n-5) + a(n-6).
a(n) = if(n<6, b(n), 3*a(n-2) - 3*a(n-4) + a(n-5) + a(n-6));
for(n=0, 25, print1(a(n)-b(n), ", "));

\\ a(n) = Sum_{k=0..floor(n/5)} binomial(n+k,n-2-5*k)
a373962(n) = sum(k=0, n\5, binomial(n+k, n-2-5*k));

\\ a(2*n) = A373962(n+2)
for(n=0, 100, print1(b(2*n)-a373962(n+2), ", "));

