M=1000;
a(n) = my(cnt); for(w=1, n\2, for(x=1, n\(2*w), for(y=1, (n-2*w*x)\x, for(z=1, (n-2*w*x-x*y)\y, m=n-2*w*x-x*y-y*z; if(m>=0 && m%(x+z)==0, cnt++))))); cnt;

for(n=1, M, write("/Users/xxx/Desktop/b375035_1.txt", n, " ", a(n)))

