M=105;

a(n) = my(cnt=0, m=n^5); for(k=1, sqrt(m/2), l=m-k*k; if(l>0&&issquare(l), cnt++)); cnt;
for(n=0, M, print1(a(n), ", "));