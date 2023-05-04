M=19;

a(n) = my(A=1); for(i=1, n, A = exp((exp(x + x*O(x^n)) - 1) * A^(x + x*O(x^n)) ) ); n!*polcoeff(A, n);
for(n=0, M, print1(a(n), ", "))
