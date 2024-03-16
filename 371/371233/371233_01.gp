M=20;

\\ a(n) = n! * Sum_{k=0..floor(n/3)} (2*n-4*k)! * |Stirling1(n-2*k,k)|/( (n-2*k)! * (2*n-5*k+1)! ).
a(n) = n! * sum(k=0, n\3, (2*n-4*k)! * abs(stirling(n-2*k,k,1))/( (n-2*k)! * (2*n-5*k+1)! ));
for(n=0, M, print1(a(n)", "))