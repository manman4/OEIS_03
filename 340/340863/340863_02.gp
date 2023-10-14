a(n) = polcoef(serlaplace(exp(n^2*x/(1-x+x*O(x^n)))/(1-x+x*O(x^n))), n);
for(n=0, 20, print1(a(n), ", "))