T(n, k) = if(n==0, 1, 2*(k%2+1)*T(n-1, k\2)+1);
for(n=0, 5, for(k=0, 2^n-1, print1(T(n, k),", ")))