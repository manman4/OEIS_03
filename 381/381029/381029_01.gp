M=21;

a(n, k=1) = if(k==0, 0^n, k*sum(j=0, n, binomial(2*n-j+k, j)/(2*n-j+k)*a(n-j, 2*j)));
for(n=0, M, print1(a(n),", "));


b(n, k) = my(A=1); for(i=1, n, A = 1/(1 - x * subst(A, x, x*A^2)^2 ) + x*O(x^n) ); polcoef(A^k, n);
for(k=1, 10, for(n=0, M, print1(a(n, k)-b(n, k),", ")); print);