\\ T(n,k) = (2*n)! * [x^(2*n)] sec(x)^k.
T(n, k) = (2*n)!*polcoef(1/cos(x + x*O(x^(2*n)))^k, 2*n);
for(n=0, 9, for(k=0, n, print1(T(k, n-k),", ")));

