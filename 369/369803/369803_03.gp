a001687(n) = if(n<0, polcoeff(x^4/(1+x^3-x^5)+x^-n*O(x), -n), polcoeff(x/(1-x^2-x^5)+x^n*O(x), n));
for(n=0, 50, print1(a001687(n), ", "));
\\ a(n) = A001687(5*n-1).
a(n) = if(n==0, 1, a001687(5*n-1));
for(n=0, 34, print1(a(n), ", "));