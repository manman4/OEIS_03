b(n, s, t) = sum(k=1, n, k^s*sigma(k, t));

f(n, m) = (subst(bernpol(m+1, x), x, n+1)-subst(bernpol(m+1, x), x, 0))/(m+1);
a(n, s=2, t=2) = sum(k=1, n, k^(s+t)*f(n\k, s));

M=33;
for(n=1, M, print1(a(n),", "))
for(n=1, M, print1(b(n, 2, 2) - a(n),", "))

a000330(n) = n*(n+1)*(2*n+1)/6;
c(n) = sum(k=1, n, k^4 * a000330(n\k));
for(n=1, M, print1(c(n) - a(n),", "))
