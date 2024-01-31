\\ T(0,k) = 1; T(n,k) = -k * (n-1)! * Sum_{j=1..min(k,n)} binomial(k-1,j-1) * T(n-j,k)/(n-j)!.
T(n,k) = if(n==0, 1, -k * (n-1)! * sum(j=1, min(k,n), binomial(k-1,j-1) * T(n-j,k)/(n-j)!));
for(n=0, 10, for(k=0, n, print1(T(k,n-k), ", "))); 