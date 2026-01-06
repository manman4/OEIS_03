\\ a(n) = Sum_{k=0..floor(2*n/5)} binomial(k,2*n-5*k).
b(n) = sum(k=0, (2*n)\5, binomial(k, 2*n-5*k));
for(n=0, 54, print1(b(n), ", "));

\\ a(n) = 2*a(n-3) + a(n-5) - a(n-6).
a(n) = if(n<6, b(n), 2*a(n-3) + a(n-5) - a(n-6));
for(n=0, 25, print1(a(n)-b(n), ", "));

