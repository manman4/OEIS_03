\\ a(n) = Sum_{k=0..floor(n/2)} binomial(k,2*(n-2*k)).
b(n) = sum(k=0, n\2, binomial(k, 2*(n-2*k)));
for(n=0, 49, print1(b(n), ", "));

\\ a(n) = 2*a(n-2) - a(n-4) + a(n-5).
a(n) = if(n<5, b(n), 2*a(n-2) - a(n-4) + a(n-5));
for(n=0, 20, print1(a(n)-b(n), ", "));

\\ a(n) = Sum_{k=0..n} C(n-k,4*k).
a005676(n) = sum(k=0, n, binomial(n-k, 4*k));

\\ a(2*n) = A005676(n)
for(n=0, 25, print1(b(2*n)-a005676(n), ", "));

\\ a(n) = Sum_{i=1..floor((n+1)/5)} binomial(n-i-1, 4i-2).
a385142(n) = sum(i=1, (n+1)\5, binomial(n-i-1, 4*i-2));

\\ a(2*n+1) = A385142(n+2).
for(n=0, 25, print1(b(2*n+1)-a385142(n+2), ", "));