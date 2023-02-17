M=48;

a(n) = if(n%2, fibonacci(n+1), [1, 0, -1][n/2%3+1]);
for(n=0, M, print1(a(n), ", "));