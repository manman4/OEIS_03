M=100;

a072213(n) = numbpart(n^2);
a206240(n) = polcoef(prod(k=1, n, 1/(1-x^k+x*O(x^(n^2-n)))), n^2-n);
a206226(n) = polcoef(prod(k=1, n, 1/(1-x^k+x*O(x^(n^2)))), n^2);
a(n) = a072213(n)+a206240(n)-a206226(n);
for(n=0, M, write("/Users/xxx/Desktop/b347585_1.txt", n, " ", a(n)))