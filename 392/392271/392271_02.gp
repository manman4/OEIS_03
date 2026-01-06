\\ a(n) = Sum_{k=0..floor(3*n/5)} binomial(k,3*n-5*k).
b(n) = sum(k=0, (3*n)\5, binomial(k, 3*n-5*k));
for(n=0, 47, print1(b(n), ", "));

\\ a(n) = 3*a(n-2) - 3*a(n-4) + a(n-5) + a(n-6).
a(n) = if(n<6, b(n), 3*a(n-2) - 3*a(n-4) + a(n-5) + a(n-6));
for(n=0, 25, print1(a(n)-b(n), ", "));

\\ a(n) = Sum_{k=0..floor(6n/5)} binomial(k, 6n-5k).
a107025(n) = sum(k=0, (6*n)\5, binomial(k, 6*n-5*k));
\\ a(2*n) = A107025(n)
for(n=0, 25, print1(b(2*n)-a107025(n), ", "));

\\ a(n) = Sum_{k=0..floor(n/5)} binomial(n+k,n-3-5*k).
a373963(n) = sum(k=0, n\5, binomial(n+k, n-3-5*k));
\\ a(2*n+1) = A373963(n+1).
for(n=0, 25, print1(b(2*n+1)-a373963(n+1), ", "));