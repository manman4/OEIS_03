T(n, k) = my(A=1+x*O(x^n)); for(i=0, n, A=1+x*A^k*(2+A)); polcoeff(A, n); 
for(n=0, 10, for(k=0, n, print1(T(k, n-k),", ")))

