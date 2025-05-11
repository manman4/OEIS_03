\\ A(n,k) = (2*n+k)!/k! * [x^(2*n+k)] sinh(x)^k.
a(n, k) = (2*n+k)!/k!*polcoef(sinh(x+x*O(x^(2*n+k)))^k, 2*n+k);
for(n=0, 15, for(k=0, n, print1(a(k, n-k), ", ")));
