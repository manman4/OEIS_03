T(n, k) = if(k==0, n!, sum(j=0, n, stirling(n, j, 2)*T(j, k-1)));
for(n=0, 15, print1(T(n,n),", "))