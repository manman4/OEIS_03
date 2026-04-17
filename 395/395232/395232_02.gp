default(realprecision, 100)

\\ A(n,k) = (2*n)! * [x^(2*n)] (f(x)^k + f(-x)^k)/2 and f(x) = 1/(cos(x) + sqrt(2) * sin(x)).
a(n, k) = my(x = 'x + O('x^(2*n+1)), f = 1/(cos(x) + sqrt(2) * sin(x))); (2*n)!*polcoef((f^k + subst(f, 'x, -x)^k)/2, 2*n);
for(n=0, 8, for(k=0, n, print1(a(k, n-k),", ")));

b(n, k) = my(x = 'x + O('x^(2*n+1)), f = 1/(cos(x) + sqrt(2) * sin(x))); round((2*n)!*polcoef((f^k + subst(f, 'x, -x)^k)/2, 2*n));
for(n=0, 8, for(k=0, n, print1(b(k, n-k),", ")));