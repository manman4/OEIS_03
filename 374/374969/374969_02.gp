M=1000;
a(n) = my(cnt=0); for(i=1, n, for(j=1, n\i, for(k=1, n\j, if(i*j+j*k<n && (n-(i*j+j*k))%(i+k)==0, cnt++)))); cnt;

for(n=1, M, write("/Users/xxx/Desktop/b374969.txt", n, " ", a(n)))

