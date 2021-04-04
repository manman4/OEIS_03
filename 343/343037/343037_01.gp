\\ \r
\\ Nは使わない
\\ 念のため精度をあげる
default(realprecision, 100);

T(n, k) = my(m=binomial(n, k)); if(issquare(m), 0, (sqrtint(m)+1)^2-m);
M=141;
cnt=2;
for(n=2, M, for(k=1, n-1, write("/Users/xxx/Desktop/b343037_1.txt", cnt, " ", T(n, k)); cnt++))