\\ a(n) = Sum_{k=0..floor(n/6)} binomial(n-4*k+1,n-6*k).
b(n) = sum(k=0, n\6, binomial(n-4*k+1, n-6*k));
for(n=0, 44, print1(b(n), ", "));

\\ a(n) = 2*a(n-1) - a(n-2) + a(n-6).
a(n) = if(n<6, b(n), 2*a(n-1) - a(n-2) + a(n-6));
for(n=0, 20, print1(a(n)-b(n), ", "));

\\ a(n) = Sum_{k=0..floor(n/3)} binomial(2*n-4*k+1,2*k+1).
a376785(n) = sum(k=0, n\3, binomial(2*n-4*k+1, 2*k+1));

\\ a(2*n) = A376785(n)
for(n=0, 100, print1(b(2*n)-a376785(n), ", "));

\\ a(n) = (1/2) * Sum_{k=0..floor(n/3)} binomial(2*n-4*k+2,2*k+1).
a375278(n) = sum(k=0, n\3, binomial(2*n-4*k+2, 2*k+1)) / 2;

\\ a(2*n+1) = 2*A375278(n).
for(n=0, 100, print1(b(2*n+1)-2*a375278(n), ", "));