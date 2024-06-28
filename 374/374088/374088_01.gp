a(n) = my(cnt=0, m=sqrtint(n)+1); for(i=1, m, for(j=1, m, if(i^2+i*j+j^2==n, cnt++))); cnt; 

for(n=0, 10000, write("/Users/xxx/Desktop/b374088_1.txt", n, " ", a(n)))

