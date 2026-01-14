\\ a(n) = Sum_{k=0..floor((2*n+1)/5)} binomial(2*k+1,2*n-5*k+1).
b(n) = sum(k=0, (2*n+1)\5, binomial(2*k+1, 2*n-5*k+1));
for(n=0, 41, print1(b(n), ", "));

\\ a(n) = 4*a(n-3) + a(n-5) - 2*a(n-6) + a(n-7).
a(n) = if(n<7, b(n), 4*a(n-3) + a(n-5) - 2*a(n-6) + a(n-7));
for(n=0, 20, print1(a(n)-b(n), ", "));

