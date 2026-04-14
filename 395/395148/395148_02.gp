\\ A(n,k) = (2*n)! * [x^(2*n)] 1/(1 - x^2)^(k/2).
a(n, k) = (2*n)!*polcoef(1/(1 - x^2 + x*O(x^(2*n)))^(k/2), 2*n);
for(n=0, 9, for(k=0, n, print1(a(k, n-k),", ")));

