\\ a(n) = (2*n)! * [x^(2*n)] 1/sqrt((1 - x)*(1 - 3*x)).
a(n) = (2*n)!*polcoef(1/sqrt((1 - x)*(1 - 3*x) + x*O(x^(2*n))), 2*n);
for(n=0, 13, print1(a(n),", "));

