T(n, k) = if(n==0, k==0, (k+1)*T(n-1, k)+(n-k)*T(n-1, k-1)+(-1)^n*(k==0));
for(n=0, 10, for(k=0, n, print1(T(n,k),", ")));

for(n=0, 20, print1(T(n,1)/2,", "));
