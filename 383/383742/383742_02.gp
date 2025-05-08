\\ g.f. x/(1 - A002203(k)*x + (-1)^k*x^2).
a002203(n) = my(w=quadgen(8)); (1+w)^n + (1-w)^n;
a(n, k) = my(x='x+O('x^(n+10))); polcoef( x/(1 - a002203(k) * x + (-1)^k * x^2), n);
for(n=0, 10, for(k=0, n, print1(a(k, n-k),", ")));