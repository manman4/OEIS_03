M=18;

a(n) = my(A=1); for(i=1, n, A = exp(x^3/6 + x * A + x*O(x^n) ) ); n!*polcoeff(A, n);
for(n=0, M, print1(a(n), ", "))
