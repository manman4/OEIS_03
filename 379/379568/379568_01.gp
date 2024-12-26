T(n, k) = my(cnt=0); for(j=10^(n-1), 10^n-1, if(numdiv(j)==k, cnt++)); cnt;
for(n=1, 9, print1(T(n, 5),", "))