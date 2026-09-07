\\ T(n,3) = 1 for n >= 3.
\\ T(4,4) = 3, and T(n,4) = 2 for n >= 5.
\\ Let l = floor((k-1)/2). For n >= k >= 5, T(n,k) = max(binomial(k-2,2) + 1, binomial(l-1,2) + (l-1)*(n-l+1) + e), where e = 1 if k is odd and e = 2 if k is even.
T(n, k) = {
  if(k==3, return(1));
  if(k==4, return(if(n==4, 3, 2)));
  my(m=(k-1)\2, e=if(k%2, 1, 2));
  max(binomial(k-2, 2)+1, binomial(m-1, 2)+(m-1)*(n-m+1)+e);
};

for(n=3, 14, for(k=3, n, print1(T(n,k),", ")));
