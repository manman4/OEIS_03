M=15;

\\ A(n,k) = n! * k! * [x^n * y^k] 1 / (1 - log(1-x) * log(1-y))^2.
a(n, k) = my(x='x+O('x^(n+1)), y='y+O('y^(k+1))); n!*k!*polcoef(polcoef( 1 / (1 - log(1-x) * log(1-y))^2, n), k);

b(n) = sum(k=0, n, k!*(k+1)!*stirling(n, k, 1)^2);
for(n=0, M, print1(b(n), ", "));

\\ a(n) == 0 (mod 2) for n > 0.
for(n=0, 50, print1(b(n)%2, ", "));

\\ Main diagonal of A382799.
for(n=0, 50, print1(a(n, n)-b(n), ", "));

a(n, k) = my(x='x+O('x^(n+1)), y='y+O('y^(k+1))); n!*k!*polcoef(polcoef( 1 / (1 - log(1+x) * log(1+y))^2, n), k);
\\ a(n) = (n!)^2 * [(x*y)^n] 1 / (1 - log(1+x) * log(1+y))^2.
for(n=0, 50, print1(a(n, n)-b(n), ", "));
