\\ \r
\\ Nは使わない
\\ M=100でもかなり時間がかかる
default(parisize, 120000000);

{a(n) = my(t='t); (2*n)!*polcoef(polcoef(prod(k=1, 2*n, (1-x^k+x*O(x^(2*n)))^(-1-t)), 2*n), n)};
M=100;
for(n=0, M, write("/Users/xxx/Desktop/b338755_gp_test.txt", n, " ", a(n)))