M=17;

\\ a(0) = 1; a(n) = Sum_{k=0..n-1} 4^(n-k) * (n-1+k)! / (k! * (n-1-k)!).
a(n) = if(n==0, 1, sum(k=0, n-1, 4^(n-k) * (n-1+k)! / (k! * (n-1-k)!)));
for(n=0, M, print1(a(n),", "))  

