default(parisize, 12000000000)

M=7;
a(n) = my(A); if( n<0, 0, A = x * O(x^n); polcoeff( eta(x^2 + A)^2 / (eta(x + A) * eta(x^4 + A)), n));

for(n=0, M, write("/Users/xxx/Desktop/b347626_1.txt", n, " ", a(n^n)))