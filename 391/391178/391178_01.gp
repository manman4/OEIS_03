\\ a(n) = Sum_{k=0..n} (k+3) * binomial(3*n-2*k+3,n-k)/(3*n-2*k+3).
a(n) = sum(k=0, n, (k+3)*binomial(3*n-2*k+3, n-k)/(3*n-2*k+3));
for(n=0, 23, print1(a(n), ", "));

\\ a(n) = A098746(n+2) - A098746(n+1).
a098746(n) = {my(k = 1); if(n > 0, k = sum(j = 0, n, (n-j)*binomial(2*j+n, j)/(2*j+n))); k; };
b(n) = a098746(n+2) - a098746(n+1);
for(n=0, 100, print1(a(n)-b(n), ", "));
