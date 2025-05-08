a002203(n) = my(w=quadgen(8)); (1+w)^n + (1-w)^n;

\\ a(n) = [x^n] x/(1 - A002203(n)*x + (-1)^n*x^2).
a(n) = my(x='x+O('x^(n+10))); polcoef( x/(1 - a002203(n) * x + (-1)^n * x^2), n);
for(n=1, 12, print1(a(n),", "))