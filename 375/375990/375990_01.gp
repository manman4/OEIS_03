\\ a(n) = -4 * (n-1)! + 8 * |Stirling1(n,2)| for n > 0.
a(n) = if(n==0, 1, -4 * (n-1)! + 8 * abs(stirling(n, 2, 1)));
for(n=0, 21, print1(a(n),", "))                           


