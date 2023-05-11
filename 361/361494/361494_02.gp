b(n) = if(n<0, 0, n!*polcoeff(1/(1-log(1+x+x*O(x^n))), n));
a(n) = sum(k=0, n, (-1)^k * stirling(n, k, 2) * b(k));
for(n=0, 21, print1(a(n),", ")) 