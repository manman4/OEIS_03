\\ A(n,k) = n! * k! * [x^n * y^k] (1/3) * (1 / (exp(x) + exp(y) - exp(x+y))^3 - 1).
a(n, k) = my(x='x+O('x^(n+1)), y='y+O('y^(k+1))); n!*k!*polcoef(polcoef( (1/3) * (1 / (exp(x) + exp(y) - exp(x+y))^3 - 1), n), k);
for(n=1, 20, for(k=1, n, print1(a(k, n-k+1)", ")));