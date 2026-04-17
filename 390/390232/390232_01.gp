default(realprecision, 100)

\\ a(n) = (2*n)! * [x^(2*n)] (1 + 2*sin(x)^2)/(1 - 4*sin(x)^2)^2.
a(n) = my(x='x+O('x^(2*n+1))); (2*n)!*polcoef((1+2*sin(x)^2)/(1-4*sin(x)^2)^2, 2*n);
for(n=0, 13, print1(a(n),", "));
