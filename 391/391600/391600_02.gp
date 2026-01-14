\\ a(n) = Sum_{k=0..floor((2*n+1)/3)} binomial(2*k+1,2*n-3*k+1).
b(n) = sum(k=0, (2*n+1)\3, binomial(2*k+1, 2*n-3*k+1));
for(n=0, 33, print1(b(n), ", "));

\\ a(n) = 4*a(n-2) + a(n-3) - 2*a(n-4) + a(n-5).
a(n) = if(n<5, b(n), 4*a(n-2) + a(n-3) - 2*a(n-4) + a(n-5));
for(n=0, 20, print1(a(n)-b(n), ", "));

