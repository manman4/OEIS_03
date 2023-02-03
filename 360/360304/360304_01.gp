M=16;

a(n) = my(A=1+x*O(x^n)); for(i=1, n, A=1-4*(n-i+1)*x/A); polcoeff(1/sqrt(A), n);
for(n=0, M, print1(a(n), ", "));