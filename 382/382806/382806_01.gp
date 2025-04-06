M=15;

\\ A(n,k) = n! * k! * [x^n * y^k] 1 / (1 - log(1-x) * log(1-y))^3.
a(n, k) = my(x='x+O('x^(n+1)), y='y+O('y^(k+1))); n!*k!*polcoef(polcoef( 1 / (1 - log(1-x) * log(1-y))^3, n), k);

b(n) = sum(k=0, n, k!^2*binomial(k+2, 2)*stirling(n, k, 1)^2);
for(n=0, M, print1(b(n), ", "));

\\ a(n) == 0 (mod 3) for n > 0.
for(n=0, 50, print1(b(n)%3, ", "));

\\ Main diagonal of A382800.
for(n=0, 50, print1(a(n, n)-b(n), ", "));



