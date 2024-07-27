M=1000;
a(n) = my(cnt=0); for(i=1, n\2, for(j=1, n\i, for(k=1, n\j-i, if(i*j+j*k<n && (n-(i*j+j*k))%(i+k)==0, cnt++)))); cnt;

for(n=1, M, write("/Users/xxx/Desktop/b374969_1.txt", n, " ", a(n)))

