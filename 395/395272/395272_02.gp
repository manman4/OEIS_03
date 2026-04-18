\\ a(n) = (2*n)! * [x^(2*n)] cos(2*x) * cos(5*x) / cos(10*x).
a(n) = my(x = 'x + O('x^(2*n+1))); (2*n)!*polcoef(cos(2*x) * cos(5*x) / cos(10*x), 2*n);
for(n=0, 15, print1(a(n),", "));   


