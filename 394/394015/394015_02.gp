\\ a(n) = (2*n)! * [x^(2*n)] cos(x) * cos(2*x) / cos(4*x).
a(n) = my(x = 'x + O('x^(2*n+1))); (2*n)!*polcoef(cos(x) * cos(2*x) / cos(4*x), 2*n);
for(n=0, 15, print1(a(n),", "));   


