\\ A(n,k) = (2*n+k)!/k! * [x^(2*n+k)] sinh(x)^k.
a381512(n, k) = (2*n+k)!/k!*polcoef(sinh(x+x*O(x^(2*n+k)))^k, 2*n+k);
\\ a(n) = A381512(n,4*n-1)
b(n) = if(n==0, 1, a381512(n,4*n-1));
for(n=0, 20, print1(b(n),", "));
