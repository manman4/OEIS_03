\\ a(n) = (2*n)! * [x^(2*n)] cosh(x) * (1 + 7*sinh(x)^2)/(1 - sinh(x)^2)^3.
a(n) = my(x='x+O('x^(2*n+1))); (2*n)!*polcoef(cosh(x)*(1+7*sinh(x)^2)/(1-sinh(x)^2)^3, 2*n);
for(n=0, 15, print1(a(n), ", "));
