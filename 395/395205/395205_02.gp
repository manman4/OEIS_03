default(realprecision, 100)

\\ a(n) = (2*n)! * [x^(2*n)] (f(x)^3 + f(-x)^3)/2 and f(x) = 1/sqrt(1 + 2*sqrt(2)*x + x^2).
a(n) = my(x = 'x + O('x^(2*n+1)), f = 1/sqrt(1 + 2*sqrt(2)*x + x^2)); (2*n)!*polcoef((f^3 + subst(f, 'x, -x)^3)/2, 2*n);
for(n=0, 15, print1(a(n),", "));

b(n) = my(x = 'x + O('x^(2*n+1)), f = 1/sqrt(1 + 2*sqrt(2)*x + x^2)); round((2*n)!*polcoef((f^3 + subst(f, 'x, -x)^3)/2, 2*n));
for(n=0, 15, print1(b(n),", "));