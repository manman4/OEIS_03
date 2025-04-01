a102365(n, k) = if(k==0, 1, if(n<k, 0, (n-k)*a102365(n-1, k-1)+(2*k+1)*a102365(n-1, k)));
for(n=0, 10, for(k=0, n, print1(a102365(n, k), ", ")));

T(n, k) = if(k<0 || k>n, 0, (n-k)*T(n-1, k-1)+2*(k+1)*T(n-1, k)+a102365(n, k));
for(n=0, 9, for(k=0, n, print1(T(n, k), ", ")));