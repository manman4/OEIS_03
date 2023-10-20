b(n, s, t) = sum(k=1, n, k^s*sigma(k, t));

f(n, m) = (subst(bernpol(m+1, x), x, n+1)-subst(bernpol(m+1, x), x, 0))/(m+1);
a(n, s=3, t=1) = sum(k=1, n, k^(s+t)*f(n\k, s));

M=33;
for(n=1, M, print1(a(n),", "))
for(n=1, M, print1(b(n, 3, 1) - a(n),", "))

a000537(n) = (n*(n+1)/2)^2;
c(n) = sum(k=1, n, k^4 * a000537(n\k));
for(n=1, M, print1(c(n) - a(n),", "))
