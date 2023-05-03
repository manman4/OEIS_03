M=17;

a(n) = my(A=1); for(i=1, n, A = exp(x / (1 - x + x*O(x^n))^2 * A ) ); n!*polcoeff(A, n);
for(n=0, M, print1(a(n), ", "))
