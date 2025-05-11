\\ A(n,k) = (2*k)! * [x^(2*k)] cosh(x)^n.
a(n, k) = (2*k)!*polcoef(cosh(x+x*O(x^(2*k)))^n, 2*k);
for(n=0, 15, print1(a(n, n), ", "));
