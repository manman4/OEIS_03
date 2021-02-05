a(n) = polcoef(serlaplace(exp(x/(1-n*x+x*O(x^n)))/(1-x+x*O(x^n))), n);
for(n=0, 50, print1(a(n), ", "))