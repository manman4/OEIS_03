\\ a(n) = (2*n)! * [x^(2*n)] (1 + 5*sinh(x)^2)/(1 - 3*sinh(x)^2)^2.
a(n) = my(x='x+O('x^(2*n+1))); (2*n)!*polcoef((1+5*sinh(x)^2)/(1-3*sinh(x)^2)^2, 2*n);
for(n=0, 15, print1(a(n), ", "));
