M=16;

\\ A(n,k) = n! * k! * [x^n * y^k] 1 / (1 - log(1-x) * log(1-y)).
a(n, k) = my(x='x+O('x^(n+1)), y='y+O('y^(k+1))); n!*k!*polcoef(polcoef( 1 / (1 - log(1-x) * log(1-y)), n), k);

b(n) = sum(k=0, n, (k!*stirling(n, k, 1))^2);
for(n=0, M, print1(b(n), ", "));

\\ Main diagonal of A379821.
for(n=0, 50, print1(a(n, n)-b(n), ", "));



