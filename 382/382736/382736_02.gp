\\ A(n,k) = n! * k! * [x^n * y^k] 1 / (exp(x) + exp(y) - exp(x+y))^4.
a(n, k) = my(x='x+O('x^(n+1)), y='y+O('y^(k+1))); n!*k!*polcoef(polcoef( 1 / (exp(x) + exp(y) - exp(x+y))^4, n), k);
for(n=0, 20, for(k=0, n, print1(a(k, n-k)", ")));