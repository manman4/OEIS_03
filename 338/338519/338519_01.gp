\\ \r
\\ Nは使わない 
f(n) = sumdiv(n, d, d*numdiv(d) == n); \\ A327166
isok(n) = f(n)==1;
cnt = 0;
for(k=1, 58180, if(isok(k), cnt++; write("C:\\Users\\xxx\\Desktop\\b338519.txt", cnt, " ", k)))