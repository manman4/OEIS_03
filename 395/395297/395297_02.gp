\\ a(n) = (2*n)! * [x^(2*n)] cosh(x)^4 / cos(x).
a(n) = my(x = 'x + O('x^(2*n+1))); (2*n)!*polcoef(cosh(x)^4 / cos(x), 2*n);
for(n=0, 15, print1(a(n),", "));   
\\ for(n=0, 15, print1(round(a(n)),", "));   

