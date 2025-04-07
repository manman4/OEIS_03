M=15;

b(n) = sum(k=0, n, binomial(n+k-1, k)*(k!*stirling(n, k, 1))^2);
for(n=0, M, print1(b(n), ", "));

\\ a(n) == 0 (mod n) for n > 0.
for(n=1, 50, print1(b(n)%n, ", "));

\\ a(n) = (n!)^2 * [(x*y)^n] 1 / (1 - log(1-x) * log(1-y))^n.
a(n) = my(x='x+O('x^(n+1)), y='y+O('y^(n+1))); n!^2*polcoef(polcoef( 1 / (1 - log(1-x) * log(1-y))^n, n), n);
for(n=0, 50, print1(a(n)-b(n), ", "));

\\ a(n) = (n!)^2 * [(x*y)^n] 1 / (1 - log(1+x) * log(1+y))^n.
a(n) = my(x='x+O('x^(n+1)), y='y+O('y^(n+1))); n!^2*polcoef(polcoef( 1 / (1 - log(1+x) * log(1+y))^n, n), n);
for(n=0, 50, print1(a(n)-b(n), ", "));



