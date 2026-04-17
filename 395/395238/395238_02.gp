\\ default(realprecision, 100)

\\ a(n) = (2*n)! * [x^(2*n)] (f(x) + f(-x))/2 and f(x) = 1/(cosh(x) - 2 * sinh(x)).
a(n) = my(x = 'x + O('x^(2*n+1)), f = 1/(cosh(x) - 2 * sinh(x))); (2*n)!*polcoef((f + subst(f, 'x, -x))/2, 2*n);
for(n=0, 15, print1(a(n),", "));

\\ b(n) = my(x = 'x + O('x^(2*n+1)), f = 1/(cosh(x) - 2 * sinh(x))); round((2*n)!*polcoef((f + subst(f, 'x, -x))/2, 2*n));
\\ for(n=0, 15, print1(b(n),", "));