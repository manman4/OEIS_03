M=17;

a(n) = 3*(3*n+2)!/(2*n+3)!;
for(n=0, M, print1(a(n), ", "))

a076151(n) = (n-1)!*binomial(3*n,n)/(3*(2*n+1));
for(n=2, M, print1(a076151(n), ", "))
\\ a(n) = 3*A076151(n+1) for n > 0.
for(n=1, M, print1(a(n) - 3 * a076151(n+1), ", "))